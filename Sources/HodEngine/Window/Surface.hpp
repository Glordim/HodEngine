#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>
#include <cstdint>

namespace hod::window
{
	/// @brief 
	class HOD_API Surface
	{
	public:
						Surface() = default;
		virtual			~Surface() = default;

	public:

		virtual void	Resize(uint32_t width, uint32_t height) = 0;
	};
}
