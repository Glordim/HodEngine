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

		struct wl_display*					GetWaylandDisplay() const;
		struct wl_surface*					GetWaylandSurface() const;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

	private:

		struct wl_surface*					_surface = nullptr;
		struct wl_shell_surface*			_shellSurface = nullptr;
	};
}

#endif
