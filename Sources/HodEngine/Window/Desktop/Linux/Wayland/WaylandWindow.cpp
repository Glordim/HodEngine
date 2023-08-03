#if defined(PLATFORM_LINUX)

#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandWindow.h"
#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.h"
#include "HodEngine/Core/Output.h"
#include "HodEngine/Core/StringConversion.h"
#include "HodEngine/Window/Desktop/Linux/Wayland/Protocols/wayland-xdg-decoration-protocol.h"

#include <string>
#include <cstring>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libdecor.h>

namespace hod::window
{
	static xdg_wm_base* _xdgWmBase = nullptr;
	static zxdg_decoration_manager_v1* _wlDecorationManager = nullptr;

	void WaylandWindow::xdg_toplevel_configure(void* data, xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, wl_array* states)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		if (width == 0 || height == 0)
		{
			/* Compositor is deferring to us */
			return;
		}
		
		waylandWindow->_width = width;
		waylandWindow->_height = height;
	}

	void WaylandWindow::xdg_toplevel_close(void* data, xdg_toplevel* toplevel)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		waylandWindow->Close();

		xdg_toplevel_destroy(toplevel);
		xdg_surface_destroy(waylandWindow->_xdgSurface);
		wl_surface_destroy(waylandWindow->_wlSurface);
	}

	static void wl_buffer_release(void* data, wl_buffer* wl_buffer)
	{
		/* Sent by the compositor when it's no longer using this buffer */
		wl_buffer_destroy(wl_buffer);
	}

	static const struct wl_buffer_listener wl_buffer_listener = {
		.release = wl_buffer_release,
	};

	static void xdg_wm_base_ping(void* data, xdg_wm_base* xdg_wm_base, uint32_t serial)
	{
		xdg_wm_base_pong(xdg_wm_base, serial);
	}

	void WaylandWindow::xdg_surface_configure(void* data, xdg_surface* xdg_surface, uint32_t serial)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		xdg_surface_ack_configure(xdg_surface, serial);

		waylandWindow->SetupBuffer();

		wl_surface_attach(waylandWindow->_wlSurface, waylandWindow->_wlBuffer, 0, 0);
		wl_surface_commit(waylandWindow->_wlSurface);
	}

	static void xdgDecorationHandleConfigure(void* userData, zxdg_toplevel_decoration_v1* decoration, uint32_t mode)
	{
		/*
		_GLFWwindow* window = userData;

		window->wl.xdg.decorationMode = mode;

		if (mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE)
		{
			if (window->decorated && !window->monitor)
				createFallbackDecorations(window);
		}
		else
			destroyFallbackDecorations(window);
		*/
	}

	void WaylandWindow::handle_configure(libdecor_frame* frame, libdecor_configuration* configuration, void* data)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		int width = waylandWindow->_width;
		int height = waylandWindow->_height;
		libdecor_window_state window_state;
		libdecor_state* state;

		/* Update window state first for the correct calculations */
		if (libdecor_configuration_get_window_state(configuration, &window_state) == false)
		{
			window_state = LIBDECOR_WINDOW_STATE_NONE;
		}

		libdecor_frame_unset_fullscreen(frame);

		//window->window_state = window_state;

		if (libdecor_configuration_get_content_size(configuration, frame, &width, &height) == false)
		{
			width = waylandWindow->_width;
			height = waylandWindow->_height;
		}
/*
		width = (width == 0) ? window->floating_width : width;
		height = (height == 0) ? window->floating_height : height;

		window->configured_width = width;
		window->configured_height = height;
*/
		state = libdecor_state_new(width, height);
		libdecor_frame_commit(frame, state, configuration);
		libdecor_state_free(state);

		/* store floating dimensions */
/*
		if (libdecor_frame_is_floating(window->frame) == true)
		{
			window->floating_width = width;
			window->floating_height = height;
		}
*/
		waylandWindow->SetupBuffer();
	}

	static void handle_close(struct libdecor_frame* frame, void* data)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		waylandWindow->Close();
	}

	static void handle_commit(struct libdecor_frame* frame, void* data)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		wl_surface_commit(waylandWindow->GetWaylandSurface());
	}

	static void handle_dismiss_popup(struct libdecor_frame* frame, const char* seat_name, void* user_data)
	{
		//popup_destroy(window->popup);
	}

	/// @brief 
	WaylandWindow::WaylandWindow()
	{
		WaylandDisplayManager* displayManager = WaylandDisplayManager::GetInstance();

		_wlSurface = wl_compositor_create_surface(displayManager->GetCompositor());
		if (_wlSurface == nullptr)
		{
			OUTPUT_ERROR("Can't create surface");
			return;
		}

		static struct libdecor_frame_interface libdecor_frame_iface = {
			handle_configure,
			handle_close,
			handle_commit,
			//handle_dismiss_popup,
		};

		libdecor_frame* frame = libdecor_decorate(displayManager->GetLibDecorContext(), _wlSurface, &libdecor_frame_iface, this);
		libdecor_frame_set_app_id(frame, "libdecor-demo");
		libdecor_frame_set_title(frame, "libdecor demo");
		libdecor_frame_map(frame);

/*
		_xdgSurface = xdg_wm_base_get_xdg_surface(_xdgWmBase, _wlSurface);
		if (_xdgSurface == nullptr)
		{
			OUTPUT_ERROR("Can't create xdg surface");
			return;
		}

		static const struct xdg_surface_listener xdg_surface_listener = {
			.configure = xdg_surface_configure,
		};

		xdg_surface_add_listener(_xdgSurface, &xdg_surface_listener, this);
		struct xdg_toplevel* xdgToplevel = xdg_surface_get_toplevel(_xdgSurface);

		static const struct xdg_toplevel_listener xdg_toplevel_listener = {
			.configure = xdg_toplevel_configure,
			.close = xdg_toplevel_close,
		};

		xdg_toplevel_add_listener(xdgToplevel, &xdg_toplevel_listener, this);
		xdg_toplevel_set_title(xdgToplevel, "HodEngine");

		if (_wlDecorationManager != nullptr) // Decoration is an optionnal extension
		{
			zxdg_toplevel_decoration_v1* wlDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(_wlDecorationManager, xdgToplevel);
			static const struct zxdg_toplevel_decoration_v1_listener xdgDecorationListener =
			{
				xdgDecorationHandleConfigure,
			};		
			zxdg_toplevel_decoration_v1_add_listener(wlDecoration, &xdgDecorationListener, this);

			uint32_t mode;

			if (true)
				mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;
			else
				mode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;

			zxdg_toplevel_decoration_v1_set_mode(wlDecoration, mode);
		}
*/
		//wl_surface_attach(_wlSurface, _wlBuffer, 0, 0);
		//wl_surface_commit(_wlSurface);

		//while (wl_display_dispatch(_wlDisplay))
		{
        	/* This space deliberately left blank */
    	}
	}

	/// @brief 
	WaylandWindow::~WaylandWindow()
	{

	}

	/// @brief 
	void WaylandWindow::Update()
	{
		
	}

	/// @brief 
	wl_surface* WaylandWindow::GetWaylandSurface() const
	{
		return _wlSurface;
	}

	/// @brief 
	/// @param width 
	/// @param height 
	void WaylandWindow::SetSize(uint16_t width, uint16_t height)
	{
		//SetupBuffer();
	}
	
	/// @brief 
	void WaylandWindow::CenterToScreen()
	{

	}

	/// @brief 
	void WaylandWindow::Maximize()
	{

	}

	/// @brief 
	bool WaylandWindow::SetupBuffer()
	{
		std::string shmName = StringConversion::StringFormat("/HodEngine_pid-%i_wlShm_window-%p", getpid(), this);
		int fd = shm_open(shmName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd < 0)
		{
			shm_unlink(shmName.c_str());
			return false;
		}

		int stride = _width * 4;
		int size = stride * _height;

		if (ftruncate(fd, size) < 0)
		{
			close(fd);
			shm_unlink(shmName.c_str());
			return false;
		}

		uint32_t* poolData = (uint32_t*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (poolData == MAP_FAILED)
		{
			close(fd);
			shm_unlink(shmName.c_str());
			return false;
		}

		WaylandDisplayManager* displayManager = WaylandDisplayManager::GetInstance();
		wl_shm_pool* pool = wl_shm_create_pool(displayManager->GetShm(), fd, size);
		_wlBuffer = wl_shm_pool_create_buffer(pool, 0, _width, _height, stride, WL_SHM_FORMAT_XRGB8888);
		wl_shm_pool_destroy(pool);

		close(fd);

		/* Draw checkerboxed background */
		for (int y = 0; y < _height; ++y) {
			for (int x = 0; x < _width; ++x) {
				if ((x + y / 8 * 8) % 16 < 8)
					poolData[y * _width + x] = 0xFF666666;
				else
					poolData[y * _width + x] = 0xFFEEEEEE;
			}
		}

		munmap(poolData, size);
		wl_buffer_add_listener(_wlBuffer, &wl_buffer_listener, nullptr);

		// todo shm_unlink ? unmap ?


		float scale = 1.0f;

		wl_surface_attach(_wlSurface, _wlBuffer, 0, 0);
		wl_surface_set_buffer_scale(_wlSurface, scale);
		wl_surface_damage_buffer(_wlSurface, 0, 0,_width * scale, _height * scale);
		wl_surface_commit(_wlSurface);

		return true;
	}
}

#endif
