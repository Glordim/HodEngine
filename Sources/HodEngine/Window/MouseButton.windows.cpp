#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/MouseButton.hpp"

namespace hod::window
{
	// Conversion table
	MouseButton windowsMouseButtonToMouseButton[6] = {
		Left, Right, Middle, Forward, Backward,
	};

	MouseButton WindowsMouseButtonToMouseButton(uint8_t scancode)
	{
		return windowsMouseButtonToMouseButton[scancode];
	}
}
