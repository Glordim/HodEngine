#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12Texture.hpp"

#include "HodEngine/RHI/D3d12/D3d12CommandBuffer.hpp"
#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline rhi
{
	namespace
	{
		constexpr DXGI_FORMAT ColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		constexpr uint32_t    ColorPixelSize = 4;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12Texture::D3d12Texture()
	: Texture()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12Texture::~D3d12Texture()
	{
		Release();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12Texture::Release()
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		if (_renderTargetViewIndex != D3d12DescriptorHeap::InvalidIndex)
		{
			rhiDevice->DeferFree(rhiDevice->GetRenderTargetViewHeap(), _renderTargetViewIndex);
			_renderTargetViewIndex = D3d12DescriptorHeap::InvalidIndex;
		}

		if (_depthStencilViewIndex != D3d12DescriptorHeap::InvalidIndex)
		{
			rhiDevice->DeferFree(rhiDevice->GetDepthStencilViewHeap(), _depthStencilViewIndex);
			_depthStencilViewIndex = D3d12DescriptorHeap::InvalidIndex;
		}

		if (_readbackBuffer != nullptr)
		{
			D3D12_RANGE writtenRange = {0, 0}; // only read
			_readbackBuffer->Unmap(0, &writtenRange);
			rhiDevice->DeferRelease(std::move(_readbackBuffer));
			_readbackBuffer = nullptr;
		}
		_readbackMappedData = nullptr;
		_readbackReady = false;

		if (_resource != nullptr)
		{
			rhiDevice->DeferRelease(std::move(_resource));
			_resource = nullptr;
		}

		_width = 0;
		_height = 0;
	}

	/// @brief
	/// @param format
	/// @return
	DXGI_FORMAT D3d12Texture::ToDxgiFormat(PixelFormat format)
	{
		switch (format)
		{
			case PixelFormat::R8G8B8A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case PixelFormat::D16_UNorm: return DXGI_FORMAT_D16_UNORM;
			default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	ID3D12Resource* D3d12Texture::GetResource() const
	{
		return _resource.Get();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE D3d12Texture::GetRenderTargetView() const
	{
		return D3d12RhiDevice::GetInstance()->GetRenderTargetViewHeap().GetCpuHandle(_renderTargetViewIndex);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE D3d12Texture::GetDepthStencilView() const
	{
		return D3d12RhiDevice::GetInstance()->GetDepthStencilViewHeap().GetCpuHandle(_depthStencilViewIndex);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12Texture::Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
	{
		if (_state == state)
		{
			return;
		}

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = _resource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = _state;
		barrier.Transition.StateAfter = state;
		commandList->ResourceBarrier(1, &barrier);

		_state = state;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12Texture::CreateResource(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state)
	{
		Release();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = flags;

		ID3D12Device* device = D3d12RhiDevice::GetInstance()->GetDevice();
		if (FAILED(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, state, nullptr, IID_PPV_ARGS(&_resource))))
		{
			OUTPUT_ERROR("D3d12: Unable to create image!");
			return false;
		}

		_state = state;
		_width = width;
		_height = height;
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12Texture::CreateReadbackBuffer()
	{
		ID3D12Device* device = D3d12RhiDevice::GetInstance()->GetDevice();

		D3D12_RESOURCE_DESC textureDesc = _resource->GetDesc();
		UINT64              readbackSize = 0;
		device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &_readbackFootprint, nullptr, nullptr, &readbackSize);

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_READBACK;

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = readbackSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_readbackBuffer))))
		{
			OUTPUT_ERROR("D3d12: Texture, unable to create readback buffer");
			return false;
		}

		if (FAILED(_readbackBuffer->Map(0, nullptr, &_readbackMappedData)))
		{
			OUTPUT_ERROR("D3d12: Texture, unable to map readback buffer");
			_readbackBuffer.Reset();
			_readbackMappedData = nullptr;
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12Texture::BuildDepth(uint32_t width, uint32_t height, const CreateInfo& /*createInfo*/)
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		// Only ever drawn into: it stays in the state that takes
		if (CreateResource(width, height, ToDxgiFormat(PixelFormat::D16_UNorm), D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
		                   D3D12_RESOURCE_STATE_DEPTH_WRITE) == false)
		{
			return false;
		}

		_depthStencilViewIndex = rhiDevice->GetDepthStencilViewHeap().Allocate();
		if (_depthStencilViewIndex == D3d12DescriptorHeap::InvalidIndex)
		{
			OUTPUT_ERROR("D3d12: Texture, the depth stencil view heap is full");
			Release();
			return false;
		}
		rhiDevice->GetDevice()->CreateDepthStencilView(_resource.Get(), nullptr, GetDepthStencilView());

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12Texture::BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo)
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		if (CreateResource(width, height, ColorFormat, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, ShaderResourceState) == false)
		{
			return false;
		}

		_renderTargetViewIndex = rhiDevice->GetRenderTargetViewHeap().Allocate();
		if (_renderTargetViewIndex == D3d12DescriptorHeap::InvalidIndex)
		{
			OUTPUT_ERROR("D3d12: Texture, the render target view heap is full");
			Release();
			return false;
		}
		rhiDevice->GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, GetRenderTargetView());

		// When _allowReadWrite is requested, readback goes through a separate persistently-mapped buffer
		if (createInfo._allowReadWrite && CreateReadbackBuffer() == false)
		{
			Release();
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12Texture::BuildBuffer(uint32_t width, uint32_t height, const uint8_t* pixels, const CreateInfo& /*createInfo*/)
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();
		ID3D12Device*   device = rhiDevice->GetDevice();

		if (CreateResource(width, height, ColorFormat, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST) == false)
		{
			return false;
		}

		ComPtr<ID3D12Resource>             uploadBuffer;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT uploadFootprint = {};

		if (pixels != nullptr)
		{
			D3D12_RESOURCE_DESC textureDesc = _resource->GetDesc();
			UINT64              uploadSize = 0;
			device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &uploadFootprint, nullptr, nullptr, &uploadSize);

			D3D12_HEAP_PROPERTIES heapProperties = {};
			heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = uploadSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			if (FAILED(
					device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer))))
			{
				OUTPUT_ERROR("D3d12: Texture, unable to create upload buffer");
				Release();
				return false;
			}

			D3D12_RANGE readRange = {0, 0};
			void*       data = nullptr;
			if (FAILED(uploadBuffer->Map(0, &readRange, &data)))
			{
				OUTPUT_ERROR("D3d12: Texture, unable to map memory");
				Release();
				return false;
			}

			// The rows of the upload buffer are further apart than the ones given
			const uint32_t rowSize = width * ColorPixelSize;
			for (uint32_t row = 0; row < height; ++row)
			{
				std::memcpy(static_cast<uint8_t*>(data) + uploadFootprint.Offset + (size_t)row * uploadFootprint.Footprint.RowPitch, pixels + (size_t)row * rowSize, rowSize);
			}
			uploadBuffer->Unmap(0, nullptr);
		}

		ID3D12GraphicsCommandList* commandList = rhiDevice->BeginImmediateCommands();
		if (commandList == nullptr)
		{
			Release();
			return false;
		}

		if (uploadBuffer != nullptr)
		{
			D3D12_TEXTURE_COPY_LOCATION destination = {};
			destination.pResource = _resource.Get();
			destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destination.SubresourceIndex = 0;

			D3D12_TEXTURE_COPY_LOCATION source = {};
			source.pResource = uploadBuffer.Get();
			source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			source.PlacedFootprint = uploadFootprint;

			commandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
		}

		Transition(commandList, ShaderResourceState);

		if (rhiDevice->EndImmediateCommands() == false) // the upload buffer is not used past it
		{
			Release();
			return false;
		}

		return true;
	}

	/// @brief
	/// @param position
	/// @return
	Color D3d12Texture::ReadPixel(const Vector2& position) const
	{
		if (position.GetX() < 0 || position.GetX() >= (float)_width || position.GetY() < 0 || position.GetY() >= (float)_height)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (_readbackReady == false)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		const uint8_t* pixelData = static_cast<const uint8_t*>(_readbackMappedData) + _readbackFootprint.Offset + (size_t)position.GetY() * _readbackFootprint.Footprint.RowPitch +
		                           (size_t)position.GetX() * ColorPixelSize;

		return Color(((float)pixelData[0]) / 255.0f, ((float)pixelData[1]) / 255.0f, ((float)pixelData[2]) / 255.0f, ((float)pixelData[3]) / 255.0f);
	}

	/// @brief Records, into commandBuffer, a copy of this texture's current content into its
	/// persistently-mapped readback buffer, so a later ReadPixel() call can read it back for free.
	/// No-op if this texture wasn't built with CreateInfo::_allowReadWrite.
	/// @param commandBuffer
	void D3d12Texture::CaptureReadback(CommandBuffer* commandBuffer)
	{
		if (_readbackBuffer == nullptr)
		{
			return;
		}

		ID3D12GraphicsCommandList* commandList = static_cast<D3d12CommandBuffer*>(commandBuffer)->GetCommandList();

		D3D12_TEXTURE_COPY_LOCATION destination = {};
		destination.pResource = _readbackBuffer.Get();
		destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		destination.PlacedFootprint = _readbackFootprint;

		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.pResource = _resource.Get();
		source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		source.SubresourceIndex = 0;

		Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
		Transition(commandList, ShaderResourceState);

		_readbackReady = true;
	}
}
