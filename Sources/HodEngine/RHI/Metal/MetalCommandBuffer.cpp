#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalCommandBuffer.hpp"

#include "HodEngine/RHI/Metal/GraphicsPipelineMetal.hpp"
#include "HodEngine/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"
#include "HodEngine/RHI/Metal/RhiDeviceMetal.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Math/Rect.hpp>

#include <Metal/Metal.hpp>

#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include <algorithm>
#include <cstring>

namespace hod::inline rhi
{
	/// @brief
	MetalCommandBuffer::MetalCommandBuffer()
	{
		RhiDeviceMetal* rhiDeviceMetal = RhiDeviceMetal::GetInstance();
		MTL::Device*   device = rhiDeviceMetal->GetDevice();

		_commandBuffer = device->newCommandBuffer();
		_commandBuffer->beginCommandBuffer(rhiDeviceMetal->GetCommandAllocator(RhiDevice::GetInstance()->GetFrameIndex()));

		MTL4::ArgumentTableDescriptor* argumentTableDescriptor = MTL4::ArgumentTableDescriptor::alloc()->init();
		argumentTableDescriptor->setMaxBufferBindCount(31);
		argumentTableDescriptor->setMaxTextureBindCount(31);
		argumentTableDescriptor->setMaxSamplerStateBindCount(16);
		NS::Error*                     error = nullptr;
		_vertexArgumentTable = device->newArgumentTable(argumentTableDescriptor, &error);
		_fragmentArgumentTable = device->newArgumentTable(argumentTableDescriptor, &error);
		argumentTableDescriptor->release();
	}

	/// @brief
	MetalCommandBuffer::~MetalCommandBuffer()
	{
		if (_renderCommandEncoder)
		{
			_renderCommandEncoder->release();
		}
		_vertexArgumentTable->release();
		_fragmentArgumentTable->release();
		_commandBuffer->release();
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::StartRecord()
	{
		return true;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::EndRecord()
	{
		_commandBuffer->endCommandBuffer();
		return true;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::StartRenderPass(RenderTarget* renderTarget, PresentationSurface* presentationSurface, const Color& color)
	{
		MTL::Texture* colorTexture = nullptr;

		if (renderTarget != nullptr)
		{
			colorTexture = static_cast<MetalTexture*>(renderTarget->GetColorTexture())->GetNativeTexture();

			Vector2 resolution = renderTarget->GetResolution();
			_renderPassWidth = (uint32_t)resolution.GetX();
			_renderPassHeight = (uint32_t)resolution.GetY();
		}
		else
		{
			MetalPresentationSurface* metalPresentationSurface = static_cast<MetalPresentationSurface*>(presentationSurface);
			CA::MetalDrawable*        drawable = metalPresentationSurface->GetCurrentDrawable();
			colorTexture = drawable->texture();

			_renderPassWidth = (uint32_t)colorTexture->width();
			_renderPassHeight = (uint32_t)colorTexture->height();
		}

		MTL4::RenderPassDescriptor* renderPassDescriptor = MTL4::RenderPassDescriptor::alloc()->init();
		renderPassDescriptor->colorAttachments()->object(0)->setTexture(colorTexture);
		renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
		renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
		renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(color.r, color.g, color.b, color.a));

		_renderCommandEncoder = _commandBuffer->renderCommandEncoder(renderPassDescriptor);
		renderPassDescriptor->release();

		_renderCommandEncoder->setArgumentTable(_vertexArgumentTable, MTL::RenderStageVertex);
		_renderCommandEncoder->setArgumentTable(_fragmentArgumentTable, MTL::RenderStageFragment);

		return true;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::EndRenderPass()
	{
		_renderCommandEncoder->endEncoding();

		return true;
	}

	/// @brief
	/// @param constant
	/// @param Size
	/// @param shaderType
	void MetalCommandBuffer::SetConstant(void* constant, uint32_t Size, Shader::ShaderType shaderType)
	{
		Buffer* constantBuffer = RhiDeviceMetal::GetInstance()->CreateBuffer(Buffer::Usage::Vertex, Size);
		void*   constantBufferData = constantBuffer->Lock();
		if (constantBufferData != nullptr)
		{
			memcpy(constantBufferData, constant, Size);
			constantBuffer->Unlock();
		}
		DeleteAfterRender(constantBuffer);

		MTL::Buffer* nativeBuffer = static_cast<MetalBuffer*>(constantBuffer)->GetNativeBuffer();
		if (shaderType == Shader::ShaderType::Vertex)
		{
			_vertexArgumentTable->setAddress(nativeBuffer->gpuAddress(), 0);
		}
		else if (shaderType == Shader::ShaderType::Fragment)
		{
			_fragmentArgumentTable->setAddress(nativeBuffer->gpuAddress(), 0);
		}
	}

	/// @brief
	/// @param projectionMatrix
	void MetalCommandBuffer::SetProjectionMatrix(const Matrix4& projectionMatrix)
	{
		// todo
		(void)projectionMatrix;
	}

	/// @brief
	/// @param viewMatrix
	void MetalCommandBuffer::SetViewMatrix(const Matrix4& viewMatrix)
	{
		// todo
		(void)viewMatrix;
	}

	/// @brief
	/// @param modelMatrix
	void MetalCommandBuffer::SetModelMatrix(const Matrix4& modelMatrix)
	{
		// todo
		(void)modelMatrix;
	}

	/// @brief
	/// @param viewport
	void MetalCommandBuffer::SetViewport(const Rect& viewport)
	{
		MTL::Viewport mtlViewport;
		mtlViewport.originX = viewport._position.GetX();
		mtlViewport.originY = viewport._position.GetY();
		mtlViewport.width = viewport._size.GetX();
		mtlViewport.height = viewport._size.GetY();
		mtlViewport.znear = 0;
		mtlViewport.zfar = 1;
		_renderCommandEncoder->setViewport(mtlViewport);
	}

	/// @brief
	/// @param scissor
	void MetalCommandBuffer::SetScissor(const Rect& scissor)
	{
		MTL::ScissorRect scissorRect;
		scissorRect.x = std::clamp((uint32_t)scissor._position.GetX(), 0u, _renderPassWidth);
		scissorRect.y = std::clamp((uint32_t)scissor._position.GetY(), 0u, _renderPassHeight);
		scissorRect.width = std::clamp((uint32_t)scissor._size.GetX(), 0u, _renderPassWidth - (uint32_t)scissorRect.x);
		scissorRect.height = std::clamp((uint32_t)scissor._size.GetY(), 0u, _renderPassHeight - (uint32_t)scissorRect.y);
		_renderCommandEncoder->setScissorRect(scissorRect);
	}

	void MetalCommandBuffer::SetGraphicsPipeline(const GraphicsPipeline* graphicsPipeline)
	{
		_graphicsPipeline = static_cast<const GraphicsPipelineMetal*>(graphicsPipeline);
		_renderCommandEncoder->setRenderPipelineState(_graphicsPipeline->GetNativeRenderPipeline());
	}

	/// @brief
	/// @param materialInstance
	/// @param setOffset
	/// @param setCount
	void MetalCommandBuffer::SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
	{
		// TODO
		(void)setOffset;
		(void)setCount;
		//
		_graphicsPipeline = static_cast<const GraphicsPipelineMetal*>(&materialInstance->GetGraphicsPipeline());
		materialInstance->ReportUnsetTextures(setOffset, setCount);
		static_cast<const MetalMaterialInstance*>(materialInstance)->FillCommandEncoder(_renderCommandEncoder, _fragmentArgumentTable);
	}

	/// @brief
	void MetalCommandBuffer::SetBindGroup(uint32_t set, const BindGroup* bindGroup, const uint32_t* uniformBufferOffsets, uint32_t uniformBufferOffsetCount)
	{
		// TODO
		(void)set;
		(void)bindGroup;
		(void)uniformBufferOffsets;
		(void)uniformBufferOffsetCount;
	}

	/// @brief
	/// @param vertexBuffer
	/// @param count
	/// @param offset
	void MetalCommandBuffer::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
	{
		NS::Range range = _graphicsPipeline->GetVertexAttributeBufferRange();

		for (uint32_t index = 0; index < count; ++index)
		{
			MTL::Buffer* mtlBuffer = static_cast<MetalBuffer*>(vertexBuffer[index])->GetNativeBuffer();
			_vertexArgumentTable->setAddress(mtlBuffer->gpuAddress() + offset, range.location + index);
		}
	}

	/// @brief
	/// @param indexBuffer
	/// @param offset
	void MetalCommandBuffer::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
	{
		_indexBuffer = static_cast<MetalBuffer*>(indexBuffer);
		_indexBufferOffset = offset;
	}

	/// @brief
	/// @param vertexCount
	void MetalCommandBuffer::Draw(uint32_t vertexCount)
	{
		// TODO primitive type from GraphicsPipeline ?
		_renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, 0, vertexCount, 1);
	}

	/// @brief
	/// @param indexCount
	/// @param indexOffset
	/// @param vertexOffset
	void MetalCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		// TODO primitive type from GraphicsPipeline ?
		MTL::GPUAddress indexBufferAddress =
			_indexBuffer->GetNativeBuffer()->gpuAddress() + indexOffset * sizeof(uint16_t) + _indexBufferOffset;
		_renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, indexBufferAddress,
														indexCount * sizeof(uint16_t), 1, static_cast<NS::Integer>(vertexOffset), 0);
	}

	/// @brief
	/// @param presentationSurface
	void MetalCommandBuffer::Present(PresentationSurface* presentationSurface)
	{
		MetalPresentationSurface* metalPresentationSurface = static_cast<MetalPresentationSurface*>(presentationSurface);
		CA::MetalDrawable*        drawable = metalPresentationSurface->GetCurrentDrawable();

		RhiDeviceMetal::GetInstance()->GetCommandQueue()->signalDrawable(drawable);
		drawable->present();
	}

	/// @brief
	/// @return
	MTL4::CommandBuffer* MetalCommandBuffer::GetNativeCommandBuffer() const
	{
		return _commandBuffer;
	}
}
