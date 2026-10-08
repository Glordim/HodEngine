#pragma once
#include "HodEngine/RHI/Export.hpp"

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API Semaphore
	{
	public:

						Semaphore() = default;
						Semaphore(const Semaphore&) = delete;
						Semaphore(Semaphore&&) = delete;
		virtual			~Semaphore() = default;

		Semaphore&		operator=(const Semaphore&) = delete;
		Semaphore&		operator=(Semaphore&&) = delete;
	};
}
