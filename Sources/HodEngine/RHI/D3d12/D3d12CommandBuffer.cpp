#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12CommandBuffer.hpp"

#include "HodEngine/RHI/D3d12/D3d12BindGroup.hpp"
#include "HodEngine/RHI/D3d12/D3d12Buffer.hpp"
#include "HodEngine/RHI/D3d12/D3d12GraphicsPipeline.hpp"
#include "HodEngine/RHI/D3d12/D3d12PresentationSurface.hpp"
#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"
#include "HodEngine/RHI/D3d12/D3d12Texture.hpp"

#include "HodEngine/RHI/RenderTarget.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Math/Rect.hpp>

#include <algorithm>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12CommandBuffer::D3d12CommandBuffer()
	{
		if (D3d12RhiDevice::GetInstance()->AcquireCommandList(_commandAllocator, _commandList) == false)
		{
			OUTPUT_ERROR("D3d12: Unable to create Command Buffer!");
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12CommandBuffer::~D3d12CommandBuffer()
	{
		// Like on the other backends, a command buffer is deleted once the GPU is done with it: its command list can serve again
		D3d12RhiDevice::GetInstance()->ReleaseCommandList(_commandAllocator, _commandList);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	ID3D12GraphicsCommandList* D3d12CommandBuffer::GetCommandList() const
	{
		return _commandList.Get();
	}

	/// @brief
	/// @return
	bool D3d12CommandBuffer::StartRecord()
	{
		if (_commandList == nullptr || FAILED(_commandAllocator->Reset()) || FAILED(_commandList->Reset(_commandAllocator.Get(), nullptr)))
		{
			OUTPUT_ERROR("D3d12: Unable to begin recording command buffer!");
			return false;
		}

		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		ID3D12DescriptorHeap* descriptorHeaps[] = {rhiDevice->GetShaderResourceHeap().GetHeap(), rhiDevice->GetSamplerHeap().GetHeap()};
		_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		_graphicsPipeline = nullptr;

		return true;
	}

	/// @brief
	/// @return
	bool D3d12CommandBuffer::EndRecord()
	{
		if (FAILED(_commandList->Close()))
		{
			OUTPUT_ERROR("D3d12: Unable to recording command buffer!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param renderTarget
	/// @param presentationSurface
	/// @param color
	bool D3d12CommandBuffer::StartRenderPass(RenderTarget* renderTarget, PresentationSurface* presentationSurface, const Color& color)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = {};
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = {};
		Vector2                     resolution;

		_renderTargetTexture = nullptr;
		_presentationSurface = nullptr;

		if (renderTarget != nullptr)
		{
			_renderTargetTexture = static_cast<D3d12Texture*>(renderTarget->GetColorTexture());
			D3d12Texture* depthTexture = static_cast<D3d12Texture*>(renderTarget->GetDepthTexture());
			if (_renderTargetTexture == nullptr || depthTexture == nullptr)
			{
				_renderTargetTexture = nullptr;
				return false;
			}

			_renderTargetTexture->Transition(_commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
			renderTargetView = _renderTargetTexture->GetRenderTargetView();
			depthStencilView = depthTexture->GetDepthStencilView();
			resolution = renderTarget->GetResolution();
		}
		else
		{
			_presentationSurface = static_cast<D3d12PresentationSurface*>(presentationSurface);
			if (_presentationSurface == nullptr || _presentationSurface->IsValid() == false)
			{
				_presentationSurface = nullptr;
				return false;
			}

			_presentationSurface->TransitionCurrentBackBuffer(_commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
			renderTargetView = _presentationSurface->GetCurrentRenderTargetView();
			depthStencilView = _presentationSurface->GetDepthStencilView();
			resolution = _presentationSurface->GetResolution();
		}

		_commandList->OMSetRenderTargets(1, &renderTargetView, FALSE, &depthStencilView);

		const float clearColor[4] = {color.r, color.g, color.b, color.a};
		_commandList->ClearRenderTargetView(renderTargetView, clearColor, 0, nullptr);
		_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr); // the farthest

		// Clip space has +y going up on D3D12, there is no need to flip the viewport as the Vulkan backend does
		D3D12_VIEWPORT d3d12Viewport = {};
		d3d12Viewport.TopLeftX = 0.0f;
		d3d12Viewport.TopLeftY = 0.0f;
		d3d12Viewport.Width = resolution.GetX();
		d3d12Viewport.Height = resolution.GetY();
		d3d12Viewport.MinDepth = 0.0f;
		d3d12Viewport.MaxDepth = 1.0f;
		_commandList->RSSetViewports(1, &d3d12Viewport);

		D3D12_RECT d3d12Scissor = {0, 0, (LONG)resolution.GetX(), (LONG)resolution.GetY()};
		_commandList->RSSetScissorRects(1, &d3d12Scissor);

		return true;
	}

	/// @brief
	bool D3d12CommandBuffer::EndRenderPass()
	{
		// What was drawn into is left ready for what comes next: to be read by a shader, or presented
		if (_renderTargetTexture != nullptr)
		{
			_renderTargetTexture->Transition(_commandList.Get(), D3d12Texture::ShaderResourceState);
			_renderTargetTexture = nullptr;
		}

		if (_presentationSurface != nullptr)
		{
			_presentationSurface->TransitionCurrentBackBuffer(_commandList.Get(), D3D12_RESOURCE_STATE_PRESENT);
			_presentationSurface = nullptr;
		}

		return true;
	}

	/// @brief
	/// @param constant
	/// @param size
	/// @param shaderType
	void D3d12CommandBuffer::SetConstant(void* constant, uint32_t size, Shader::ShaderType /*shaderType*/)
	{
		if (_graphicsPipeline == nullptr || _graphicsPipeline->GetPushConstantRootParameter() == D3d12GraphicsPipeline::NoRootParameter)
		{
			return;
		}

		size = std::min(size, _graphicsPipeline->GetPushConstantSize());

		_commandList->SetGraphicsRoot32BitConstants(_graphicsPipeline->GetPushConstantRootParameter(), size / sizeof(uint32_t), constant, 0);
	}

	/// @brief
	/// @param viewport
	void D3d12CommandBuffer::SetViewport(const Rect& viewport)
	{
		D3D12_VIEWPORT d3d12Viewport = {};
		d3d12Viewport.TopLeftX = viewport._position.GetX();
		d3d12Viewport.TopLeftY = viewport._position.GetY();
		d3d12Viewport.Width = viewport._size.GetX();
		d3d12Viewport.Height = viewport._size.GetY();
		d3d12Viewport.MinDepth = 0.0f;
		d3d12Viewport.MaxDepth = 1.0f;

		_commandList->RSSetViewports(1, &d3d12Viewport);
	}

	/// @brief
	/// @param scissor
	void D3d12CommandBuffer::SetScissor(const Rect& scissor)
	{
		D3D12_RECT d3d12Scissor;
		d3d12Scissor.left = std::max((LONG)scissor._position.GetX(), (LONG)0);
		d3d12Scissor.top = std::max((LONG)scissor._position.GetY(), (LONG)0);
		d3d12Scissor.right = d3d12Scissor.left + (LONG)scissor._size.GetX();
		d3d12Scissor.bottom = d3d12Scissor.top + (LONG)scissor._size.GetY();

		_commandList->RSSetScissorRects(1, &d3d12Scissor);
	}

	/// @brief
	/// @param graphicsPipeline
	void D3d12CommandBuffer::SetGraphicsPipeline(const GraphicsPipeline* graphicsPipeline)
	{
		const D3d12GraphicsPipeline* d3d12GraphicsPipeline = static_cast<const D3d12GraphicsPipeline*>(graphicsPipeline);
		if (_graphicsPipeline != d3d12GraphicsPipeline)
		{
			_graphicsPipeline = d3d12GraphicsPipeline;

			// A new root signature unbinds what was bound to the previous one: like with a new pipeline layout on Vulkan,
			// the push constant and the BindGroups are to be set again
			_commandList->SetGraphicsRootSignature(_graphicsPipeline->GetRootSignature());
			_commandList->SetPipelineState(_graphicsPipeline->GetPipelineState());
			_commandList->IASetPrimitiveTopology(_graphicsPipeline->GetPrimitiveTopology());
		}
	}

	/// @brief
	/// @param set
	/// @param bindGroup
	/// @param uniformBufferOffsets
	/// @param uniformBufferOffsetCount
	void D3d12CommandBuffer::SetBindGroup(uint32_t set, const BindGroup* bindGroup, const uint32_t* uniformBufferOffsets, uint32_t uniformBufferOffsetCount)
	{
		Assert(_graphicsPipeline != nullptr); // the set is bound against the root signature of the current pipeline

		const D3d12GraphicsPipeline::SetLayout* setLayout = _graphicsPipeline->GetSetLayout(set);
		if (setLayout == nullptr)
		{
			return;
		}

		const D3d12BindGroup*                    d3d12BindGroup = static_cast<const D3d12BindGroup*>(bindGroup);
		const Vector<D3D12_GPU_VIRTUAL_ADDRESS>& uniformBufferAddresses = d3d12BindGroup->GetUniformBufferAddresses();
		Assert(uniformBufferOffsetCount == uniformBufferAddresses.Size());

		for (uint32_t index = 0; index < uniformBufferAddresses.Size() && index < uniformBufferOffsetCount; ++index)
		{
			_commandList->SetGraphicsRootConstantBufferView(setLayout->_firstUniformBufferRootParameter + index, uniformBufferAddresses[index] + uniformBufferOffsets[index]);
		}

		if (setLayout->_textureTableRootParameter != D3d12GraphicsPipeline::NoRootParameter)
		{
			_commandList->SetGraphicsRootDescriptorTable(setLayout->_textureTableRootParameter, d3d12BindGroup->GetTextureTable());
		}

		if (setLayout->_samplerTableRootParameter != D3d12GraphicsPipeline::NoRootParameter)
		{
			_commandList->SetGraphicsRootDescriptorTable(setLayout->_samplerTableRootParameter, d3d12BindGroup->GetSamplerTable());
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12CommandBuffer::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
	{
		Assert(_graphicsPipeline != nullptr); // the size of a vertex comes from the vertex inputs of the current pipeline

		D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		count = std::min<uint32_t>(count, D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
		for (uint32_t index = 0; index < count; ++index)
		{
			const D3d12Buffer* d3d12Buffer = static_cast<const D3d12Buffer*>(vertexBuffer[index]);

			vertexBufferViews[index].BufferLocation = d3d12Buffer->GetGpuAddress() + offset;
			vertexBufferViews[index].SizeInBytes = d3d12Buffer->GetSize() > offset ? d3d12Buffer->GetSize() - offset : 0;
			vertexBufferViews[index].StrideInBytes = _graphicsPipeline != nullptr ? _graphicsPipeline->GetVertexStride(index) : 0;
		}

		_commandList->IASetVertexBuffers(0, count, vertexBufferViews);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12CommandBuffer::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
	{
		const D3d12Buffer* d3d12Buffer = static_cast<const D3d12Buffer*>(indexBuffer);

		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		indexBufferView.BufferLocation = d3d12Buffer->GetGpuAddress() + offset;
		indexBufferView.SizeInBytes = d3d12Buffer->GetSize() > offset ? d3d12Buffer->GetSize() - offset : 0;
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;

		_commandList->IASetIndexBuffer(&indexBufferView);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12CommandBuffer::Draw(uint32_t vertexCount)
	{
		if (_graphicsPipeline != nullptr && _graphicsPipeline->IsTriangleFanEmulated())
		{
			DrawEmulatedTriangleFan(vertexCount);
			return;
		}

		_commandList->DrawInstanced(vertexCount, 1, 0, 0);
	}

	/// @brief Triangle fans are an optional feature of D3D12: without it, the fan is drawn as the list of its triangles.
	/// Takes the place of the index buffer that was set.
	/// @param vertexCount
	void D3d12CommandBuffer::DrawEmulatedTriangleFan(uint32_t vertexCount)
	{
		if (vertexCount < 3)
		{
			return;
		}

		uint32_t indexCount = (vertexCount - 2) * 3;

		Buffer*   indexBuffer = D3d12RhiDevice::GetInstance()->CreateBuffer(Buffer::Usage::Index, indexCount * sizeof(uint16_t));
		uint16_t* indices = static_cast<uint16_t*>(indexBuffer->Lock());
		DeleteAfterRender(indexBuffer);
		if (indices == nullptr)
		{
			return;
		}

		for (uint32_t triangle = 0; triangle < vertexCount - 2; ++triangle)
		{
			indices[triangle * 3 + 0] = 0;
			indices[triangle * 3 + 1] = (uint16_t)(triangle + 1);
			indices[triangle * 3 + 2] = (uint16_t)(triangle + 2);
		}
		indexBuffer->Unlock();

		SetIndexBuffer(indexBuffer);
		_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		// Indexed fans are not emulated: nothing draws any, and it would take reading the index buffer back
		Assert(_graphicsPipeline == nullptr || _graphicsPipeline->IsTriangleFanEmulated() == false);

		_commandList->DrawIndexedInstanced(indexCount, 1, indexOffset, (INT)vertexOffset, 0);
	}

	/// @brief Nothing to record: the back buffer is made presentable by EndRenderPass, and presented by D3d12PresentationSurface::SwapBuffer
	void D3d12CommandBuffer::Present(PresentationSurface* /*presentationSurface*/) {}
}
