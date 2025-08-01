#pragma once
#include "HodEngine/Window/Export.hpp"

namespace hod::window
{
	class HOD_WINDOW_API IDesktopWindowInputListener
	{
	public:
		virtual ~IDesktopWindowInputListener() = default;

		virtual void OnKeyPressed(int keyCode) = 0;
		virtual void OnKeyReleased(int keyCode) = 0;
		virtual void OnMouseButtonPressed(int button) = 0;
		virtual void OnMouseButtonReleased(int button) = 0;
		virtual void OnMouseMoved(float x, float y) = 0;
	};
}
