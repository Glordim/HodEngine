#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/MouseButton.hpp"
#include "HodEngine/Window/ScanCode.hpp"

namespace hod::window
{
	class HOD_WINDOW_API IDesktopWindowInputListener
	{
	public:
		virtual ~IDesktopWindowInputListener() = default;

		virtual void OnKeyPressed(ScanCode scanCode) = 0;
		virtual void OnKeyReleased(ScanCode scanCode) = 0;
		virtual void OnMouseButtonPressed(MouseButton button) = 0;
		virtual void OnMouseButtonReleased(MouseButton button) = 0;
		virtual void OnMouseMoved(float x, float y) = 0;
		virtual void OnMouseScroll(float scroll) = 0;
	};
}
