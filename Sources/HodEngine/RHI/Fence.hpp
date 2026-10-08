#pragma once
#include "HodEngine/RHI/Export.hpp"

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API Fence
	{
	public:

						Fence() = default;
						Fence(const Fence&) = delete;
						Fence(Fence&&) = delete;
		virtual			~Fence() = default;

		Fence&			operator=(const Fence&) = delete;
		Fence&			operator=(Fence&&) = delete;

	public:

		virtual bool	Reset() = 0;
		virtual bool	Wait() = 0;
	};
}
