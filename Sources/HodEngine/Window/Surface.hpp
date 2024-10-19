#pragma once
#include "HodEngine/Window/Export.hpp"

#include <string>
#include <cstdint>

namespace hod::window
{
	/// @brief 
	class HOD_WINDOW_API Surface
	{
	public:
						Surface() = default;
		virtual			~Surface() = default;

	public:

		virtual void		Resize(uint32_t width, uint32_t height) = 0;
		virtual	uint32_t	GetWidth() = 0;
		virtual uint32_t	GetHeight() = 0;
	};
}
