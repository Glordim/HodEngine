#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Semaphore.hpp"

namespace hod::inline rhi
{
	/// @brief Nothing behind it: the command queue already executes what it is given in order,
	/// see D3d12RhiDevice::SubmitCommandBuffers
	class HOD_RHI_API D3d12Semaphore : public Semaphore
	{
	public:
		D3d12Semaphore() = default;
		D3d12Semaphore(const D3d12Semaphore&) = delete;
		D3d12Semaphore(D3d12Semaphore&&) = delete;
		~D3d12Semaphore() override = default;

		D3d12Semaphore& operator=(const D3d12Semaphore&) = delete;
		D3d12Semaphore& operator=(D3d12Semaphore&&) = delete;
	};
}
