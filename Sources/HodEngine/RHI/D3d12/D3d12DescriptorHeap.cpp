#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12DescriptorHeap.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	/// @brief
	/// @param device
	/// @param type
	/// @param capacity
	/// @param shaderVisible
	/// @return
	bool D3d12DescriptorHeap::Init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool shaderVisible)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = type;
		heapDesc.NumDescriptors = capacity;
		heapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_heap))))
		{
			OUTPUT_ERROR("D3d12: Unable to create a descriptor heap of {} descriptors!", capacity);
			return false;
		}

		_cpuStart = _heap->GetCPUDescriptorHandleForHeapStart();
		if (shaderVisible)
		{
			_gpuStart = _heap->GetGPUDescriptorHandleForHeapStart();
		}
		_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
		_capacity = capacity;
		_next = 0;
		_freeBlocks.Clear();

		return true;
	}

	/// @brief
	void D3d12DescriptorHeap::Release()
	{
		_heap.Reset();
		_freeBlocks.Clear();
		_capacity = 0;
		_next = 0;
	}

	/// @brief
	/// @param count
	/// @return
	uint32_t D3d12DescriptorHeap::Allocate(uint32_t count)
	{
		Assert(count > 0);

		if (count <= _freeBlocks.Size() && _freeBlocks[count - 1].Empty() == false)
		{
			Vector<uint32_t>& freeBlocks = _freeBlocks[count - 1];
			uint32_t          index = freeBlocks.Back();
			freeBlocks.PopBack();
			return index;
		}

		if (_next + count > _capacity)
		{
			return InvalidIndex;
		}

		uint32_t index = _next;
		_next += count;
		return index;
	}

	/// @brief
	/// @param index
	/// @param count
	void D3d12DescriptorHeap::Free(uint32_t index, uint32_t count)
	{
		Assert(count > 0 && index + count <= _next);

		if (_freeBlocks.Size() < count)
		{
			_freeBlocks.Resize(count);
		}
		_freeBlocks[count - 1].PushBack(index);
	}

	/// @brief
	/// @param index
	/// @return
	D3D12_CPU_DESCRIPTOR_HANDLE D3d12DescriptorHeap::GetCpuHandle(uint32_t index) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = _cpuStart;
		handle.ptr += (SIZE_T)index * _descriptorSize;
		return handle;
	}

	/// @brief
	/// @param index
	/// @return
	D3D12_GPU_DESCRIPTOR_HANDLE D3d12DescriptorHeap::GetGpuHandle(uint32_t index) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = _gpuStart;
		handle.ptr += (UINT64)index * _descriptorSize;
		return handle;
	}

	/// @brief
	/// @return
	ID3D12DescriptorHeap* D3d12DescriptorHeap::GetHeap() const
	{
		return _heap.Get();
	}
}
