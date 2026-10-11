#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12Buffer.hpp"
#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12Buffer::D3d12Buffer(Usage usage, uint32_t size)
	: Buffer(usage, size)
	{
		Resize(size);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12Buffer::~D3d12Buffer()
	{
		Release();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12Buffer::Release()
	{
		if (_resource != nullptr)
		{
			// A frame in flight may still read it
			D3d12RhiDevice::GetInstance()->DeferRelease(std::move(_resource));
			_resource = nullptr;
		}

		_size = 0;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	ID3D12Resource* D3d12Buffer::GetResource() const
	{
		return _resource.Get();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3D12_GPU_VIRTUAL_ADDRESS D3d12Buffer::GetGpuAddress() const
	{
		return _resource != nullptr ? _resource->GetGPUVirtualAddress() : 0;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	uint32_t D3d12Buffer::GetSize() const
	{
		return _size;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12Buffer::Resize(uint32_t size)
	{
		Release();

		// A constant buffer is read by blocks of D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT bytes, whatever the size of what the shader declares
		uint64_t resourceSize = size > 0 ? size : 1;
		if (_usage == Usage::Uniform)
		{
			const uint64_t alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			resourceSize = (resourceSize + alignment - 1) / alignment * alignment;
		}

		// The buffers of the RHI are all written by the CPU, like the CPU_TO_GPU ones of the Vulkan backend
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = resourceSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ID3D12Device* device = D3d12RhiDevice::GetInstance()->GetDevice();
		if (FAILED(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_resource))))
		{
			OUTPUT_ERROR("D3d12: Unable to create buffer!");
			return false;
		}

		_size = size;
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void* D3d12Buffer::Lock()
	{
		if (_resource == nullptr)
		{
			return nullptr;
		}

		D3D12_RANGE readRange = {0, 0}; // nothing is read back

		void* data = nullptr;
		if (FAILED(_resource->Map(0, &readRange, &data)))
		{
			OUTPUT_ERROR("D3d12: Unable to map buffer memory!");
			return nullptr;
		}

		return data;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12Buffer::Unlock()
	{
		if (_resource != nullptr)
		{
			_resource->Unmap(0, nullptr);
		}
	}
}
