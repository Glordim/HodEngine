#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/MouseButton.hpp"

namespace hod::window
{
	// Conversion table
	MouseButton windowsMouseButtonToMouseButton[6] = {
		MouseButton::Left, MouseButton::Right, MouseButton::Middle, MouseButton::Forward, MouseButton::Backward,
	};

	MouseButton WindowsMouseButtonToMouseButton(uint8_t scancode)
	{
		return windowsMouseButtonToMouseButton[scancode];
	}
}
