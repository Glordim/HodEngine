#pragma once
#include "HodEngine/Window/Export.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>
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

		virtual void		Resize(uint32_t width, uint32_t height) = 0; // todo Vector2 size ?
		virtual Vector2		GetResolution() = 0;
	};
}
