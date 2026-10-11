#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Fence.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API D3d12Fence : public Fence
	{
	public:
		D3d12Fence();
		D3d12Fence(const D3d12Fence&) = delete;
		D3d12Fence(D3d12Fence&&) = delete;
		~D3d12Fence() override;

		D3d12Fence& operator=(const D3d12Fence&) = delete;
		D3d12Fence& operator=(D3d12Fence&&) = delete;

	public:
		bool Reset() override;
		bool Wait() override;

		// Wait will return once the queue is done with everything it was given so far
		bool Signal(ID3D12CommandQueue* commandQueue) const;

	private:
		ComPtr<ID3D12Fence> _fence;
		HANDLE              _event = nullptr;
		mutable uint64_t    _value = 0; // last value the queue was asked to signal
	};
}
