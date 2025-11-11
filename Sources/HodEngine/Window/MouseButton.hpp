#pragma once
#include "HodEngine/Window/Export.hpp"

#include <cstdint>

namespace hod::window
{
	/// @brief CrossPlatform enum
	enum class MouseButton : uint8_t
	{
		Left,
		Right,
		Middle,
		Forward,
		Backward,
	};

#if defined(PLATFORM_WINDOWS)
	HOD_WINDOW_API MouseButton WindowsMouseButtonToMouseButton(uint8_t mouseButton);
#endif
}
