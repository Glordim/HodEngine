#pragma once

#if defined(PLATFORM_LINUX)

#include "HodEngine/Window/DesktopWindow/DesktopWindow.h"

#include <wayland-client.h>
#include <wayland-client-protocol.h>

namespace hod::window
{
	/// @brief 
	class WaylandWindow : public DesktopWindow
	{
	public:
											WaylandWindow();
											~WaylandWindow() override;

		void								Update() override;

		WindowHandle						GetWindowHandle() const override;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

	private:

		WindowHandle						_hWnd = nullptr;

		struct wl_surface*					_surface = nullptr;
		struct wl_shell_surface*			_shellSurface = nullptr;
	};
}

#endif
