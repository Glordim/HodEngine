#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	/// @brief A descriptor heap handing out blocks of contiguous descriptors.
	/// A freed block is only given back to a request of the same size, which fits how the RHI uses it:
	/// single descriptors for the views, and one block per BindGroup, whose sizes are the few a set can have.
	class HOD_RHI_API D3d12DescriptorHeap
	{
	public:
		static constexpr uint32_t InvalidIndex = 0xFFFFFFFF;

	public:
		D3d12DescriptorHeap() = default;
		D3d12DescriptorHeap(const D3d12DescriptorHeap&) = delete;
		D3d12DescriptorHeap(D3d12DescriptorHeap&&) = delete;
		~D3d12DescriptorHeap() = default;

		D3d12DescriptorHeap& operator=(const D3d12DescriptorHeap&) = delete;
		D3d12DescriptorHeap& operator=(D3d12DescriptorHeap&&) = delete;

	public:
		bool Init(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool shaderVisible);
		void Release();

		// Index of the first descriptor of the block, InvalidIndex when the heap is full
		uint32_t Allocate(uint32_t count = 1);
		void     Free(uint32_t index, uint32_t count = 1);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t index) const; // only for a shader visible heap

		ID3D12DescriptorHeap* GetHeap() const;

	private:
		ComPtr<ID3D12DescriptorHeap> _heap;
		D3D12_CPU_DESCRIPTOR_HANDLE  _cpuStart = {};
		D3D12_GPU_DESCRIPTOR_HANDLE  _gpuStart = {};
		uint32_t                     _descriptorSize = 0;
		uint32_t                     _capacity = 0;
		uint32_t                     _next = 0; // first descriptor never allocated

		Vector<Vector<uint32_t>> _freeBlocks; // freed blocks of 'index + 1' descriptors
	};
}
