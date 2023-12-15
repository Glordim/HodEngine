#pragma once

#if defined(PLATFORM_LINUX)

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "HodEngine/Window/Desktop/Linux/Wayland/Protocols/wayland-xdg-shell-client-protocol.hpp"
#include <libdecor-0/libdecor.h>

// todo avoid to use libDecor when it's possible, lot of compositor support server side decoration (except gnome...)

namespace hod::window
{
	/// @brief 
	class WaylandWindow : public DesktopWindow
	{
	public:
											WaylandWindow();
											~WaylandWindow() override;

		void								Update() override;

		wl_surface*							GetWaylandSurface() const;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

	private:

		enum RequestFlag
		{
			NeedResize = (1 << 0),
			NeedResizeReady = (1 << 1),
		};

		struct Buffer
		{
			wl_buffer*	_wlBuffer = nullptr;
			void*		_data = nullptr;
			size_t		_size = 0;
		};		

	private:

		bool								SetupBuffer();

		static void 						xdg_surface_configure(void* data, xdg_surface* xdg_surface, uint32_t serial);
		static void							xdg_toplevel_configure(void* data, xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, struct wl_array* states);
		static void							xdg_toplevel_close(void* data, xdg_toplevel* toplevel);
		static void							handle_configure(libdecor_frame* frame, libdecor_configuration* configuration, void* data);

		static void							BufferRelease(void* userData, wl_buffer* wl_buffer);

	private:

		wl_surface*					_wlSurface = nullptr;
		wl_buffer*					_wlBuffer = nullptr;
		xdg_surface*				_xdgSurface = nullptr;

		uint32_t					_requestFlags = 0;
		uint32_t					_requestedWidth = 0;
		uint32_t					_requestedHeight = 0;
	};
}

#endif
