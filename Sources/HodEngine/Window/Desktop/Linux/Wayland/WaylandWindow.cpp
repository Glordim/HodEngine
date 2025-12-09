#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/StringConversion.hpp"
#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp"
#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandWindow.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstring>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libdecor.h>

namespace hod::window
{
	void WaylandWindow::xdg_toplevel_configure(void* /*userData*/, xdg_toplevel* /*xdg_toplevel*/, int32_t width, int32_t height, wl_array* /*states*/)
	{
		//WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(userData);
		if (width == 0 || height == 0)
		{
			// Compositor is deferring to us
			return;
		}
		/*
		waylandWindow->_requestedWidth = width;
		waylandWindow->_requestedHeight = height;
		waylandWindow->_requestFlags |= RequestFlag::NeedResize;
		*/
	}

	void WaylandWindow::xdg_toplevel_close(void* data, xdg_toplevel* toplevel)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		waylandWindow->Close();

		xdg_toplevel_destroy(toplevel);
		xdg_surface_destroy(waylandWindow->_xdgSurface);
		wl_surface_destroy(waylandWindow->_wlSurface);
	}

	/// @brief Sent by the compositor when it's no longer using this buffer
	/// @param userData
	/// @param wl_buffer
	void WaylandWindow::BufferRelease(void* userData, wl_buffer* /*wl_buffer*/)
	{
		Buffer* buffer = (Buffer*)userData;

		wl_buffer_destroy(buffer->_wlBuffer);
		munmap(buffer->_data, buffer->_size);
		DefaultAllocator::GetInstance().Delete(buffer);
	}

	void WaylandWindow::xdg_surface_configure(void* userData, xdg_surface* xdg_surface, uint32_t serial)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(userData);

		xdg_surface_ack_configure(xdg_surface, serial);

		if (waylandWindow->_requestFlags & RequestFlag::NeedResize)
		{
			waylandWindow->_requestFlags &= ~(RequestFlag::NeedResize);
			waylandWindow->_requestFlags |= RequestFlag::NeedResizeReady;
		}
		/*
		        waylandWindow->SetupBuffer();

		        wl_surface_attach(waylandWindow->_wlSurface, waylandWindow->_wlBuffer, 0, 0);
		        wl_surface_commit(waylandWindow->_wlSurface);
		*/
	}

	//static void xdgDecorationHandleConfigure(void* /*userData*/, zxdg_toplevel_decoration_v1* /*decoration*/, uint32_t /*mode*/)
	//{
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
	//}

	void WaylandWindow::handle_configure(libdecor_frame* frame, libdecor_configuration* configuration, void* data)
	{
		static_cast<WaylandWindow*>(data)->HandleConfigure(frame, configuration);
	}

	void WaylandWindow::HandleConfigure(libdecor_frame* frame, libdecor_configuration* configuration)
	{
		int newWidth;
		int newHeight;
		if (libdecor_configuration_get_content_size(configuration, frame, &newWidth, &newHeight) == false)
		{
			newWidth = _width;
			newHeight = _height;
		}
		libdecor_state* state = libdecor_state_new(newWidth, newHeight);
		libdecor_frame_commit(frame, state, configuration);
		libdecor_state_free(state);

		if (newWidth != _width || newHeight != _height)
		{
			_width = newWidth;
			_height = newHeight;
			
			Event event;
			event.type = EventType::Resized;
			event.data.resize.width = newWidth;
			event.data.resize.height = newHeight;
			EnqueueEvent(event);
		}
	}

	static void handle_close(struct libdecor_frame* /*frame*/, void* data)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		waylandWindow->Close();
	}

	static void handle_commit(struct libdecor_frame* /*frame*/, void* data)
	{
		WaylandWindow* waylandWindow = static_cast<WaylandWindow*>(data);
		wl_surface_commit(waylandWindow->GetWaylandSurface());
	}

	//static void handle_dismiss_popup(struct libdecor_frame* /*frame*/, const char* /*seat_name*/, void* /*user_data*/)
	//{
		// popup_destroy(window->popup);
	//}

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

		libdecor* libdecorContext = displayManager->GetLibDecorContext();
		if (libdecorContext != nullptr)
		{
			static struct libdecor_frame_interface libdecor_frame_iface = {
				.configure = handle_configure,
				.close = handle_close,
				.commit = handle_commit,
				.dismiss_popup = nullptr, // handle_dismiss_popup,
				.reserved0 = nullptr,
				.reserved1 = nullptr,
				.reserved2 = nullptr,
				.reserved3 = nullptr,
				.reserved4 = nullptr,
				.reserved5 = nullptr,
				.reserved6 = nullptr,
				.reserved7 = nullptr,
				.reserved8 = nullptr,
				.reserved9 = nullptr,
			};

			_frame = libdecor_decorate(displayManager->GetLibDecorContext(), _wlSurface, &libdecor_frame_iface, this);
			libdecor_frame_set_title(_frame, "HodEngine");
			libdecor_frame_set_app_id(_frame, "HodEngine");
			libdecor_frame_map(_frame);
		}
		else
		{
			_xdgSurface = xdg_wm_base_get_xdg_surface(displayManager->GetXdgWmBase(), _wlSurface);
			if (_xdgSurface == nullptr)
			{
				OUTPUT_ERROR("Can't create xdg surface");
				return;
			}

			static const xdg_surface_listener xdg_surface_listener = {
				.configure = xdg_surface_configure,
			};

			xdg_surface_add_listener(_xdgSurface, &xdg_surface_listener, this);
			xdg_toplevel* xdgToplevel = xdg_surface_get_toplevel(_xdgSurface);

			static const xdg_toplevel_listener xdg_toplevel_listener = {
				.configure = xdg_toplevel_configure,
				.close = xdg_toplevel_close,
				.configure_bounds = nullptr,
				.wm_capabilities = nullptr,
			};

			xdg_toplevel_add_listener(xdgToplevel, &xdg_toplevel_listener, this);
			xdg_toplevel_set_title(xdgToplevel, "HodEngine");
			xdg_toplevel_set_app_id(xdgToplevel, "HodEngine");

			/*
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
			*/
		}

		wl_surface_commit(_wlSurface);
		wl_display_roundtrip(displayManager->GetDisplay());
		wl_surface_commit(_wlSurface);
	}

	/// @brief
	WaylandWindow::~WaylandWindow() {}

	/// @brief
	void WaylandWindow::Update()
	{
		DesktopWindow::Update();
		/*
		if (_requestFlags & RequestFlag::NeedResizeReady)
		{
		    SetSize(_requestedWidth, _requestedHeight);
		    _requestFlags &= ~(RequestFlag::NeedResizeReady);
		}
		*/
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
		/*
		wl_surface_set_buffer_scale(_wlSurface, 1);
		wl_surface_attach(_wlSurface, _wlBuffer, 0, 0);
		wl_surface_damage(_wlSurface, 0, 0, width, height);
		wl_surface_commit(_wlSurface);
		*/

		struct libdecor_state* state;
		state = libdecor_state_new(width, height);
		libdecor_frame_commit(_frame, state, NULL);
		libdecor_state_free(state);

		SetupBuffer();

		_width = width;
		_height = height;

		/*
		//wl_surface_set_buffer_scale(_wlSurface, 1);  // Exemple d'échelle
		//wl_surface_attach(_wlSurface, _wlBuffer, 0, 0); // Attacher le buffer
		wl_surface_damage(_wlSurface, 0, 0, width, height);
		wl_surface_commit(_wlSurface);
		//SetupBuffer();
		GetSurface()->Resize(width, height);
		_width = width;
		_height = height;
		_requestedWidth = width;
		_requestedHeight = height;

		wl_surface_commit(_wlSurface);
		*/
	}

	/// @brief
	void WaylandWindow::CenterToScreen() {}

	/// @brief
	void WaylandWindow::Maximize()
	{
		libdecor_frame_set_maximized(_frame);
	}

	/// @brief
	/// @param visible
	void WaylandWindow::SetVisible(bool visible)
	{
		libdecor_frame_set_visibility(_frame, visible);
	}

	/// @brief
	bool WaylandWindow::SetupBuffer()
	{
		/*
		String shmName = StringConversion::StringFormat("/HodEngine_pid-%i_wlShm_window-%p", getpid(), this);
		int fd = shm_open(shmName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd < 0)
		{
		    return false;
		}

		int stride = _width * 4;
		int Size = stride * _height;

		if (ftruncate(fd, Size) < 0)
		{
		    close(fd);
		    return false;
		}

		uint32_t* poolData = (uint32_t*)mmap(nullptr, Size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (poolData == MAP_FAILED)
		{
		    close(fd);
		    return false;
		}

		WaylandDisplayManager* displayManager = WaylandDisplayManager::GetInstance();
		wl_shm_pool* pool = wl_shm_create_pool(displayManager->GetShm(), fd, Size);


		Buffer* buffer = DefaultAllocator::GetInstance().New<Buffer>();
		buffer->_wlBuffer = wl_shm_pool_create_buffer(pool, 0, _width, _height, stride, WL_SHM_FORMAT_XRGB8888);
		buffer->_data = poolData;
		buffer->_size = Size;

		static const wl_buffer_listener bufferListener = {
		    .release = &WaylandWindow::BufferRelease,
		};
		wl_buffer_add_listener(buffer->_wlBuffer, &bufferListener, buffer);
		wl_shm_pool_destroy(pool);
		close(fd);

		// Draw checkerboxed background
		for (int y = 0; y < _height; ++y) {
		    for (int x = 0; x < _width; ++x) {
		        if ((x + y / 8 * 8) % 16 < 8)
		            poolData[y * _width + x] = 0xFF666666;
		        else
		            poolData[y * _width + x] = 0xFFEEEEEE;
		    }
		}

		//munmap(poolData, Size);

		// todo shm_unlink ? unmap ?


		float scale = 1.0f;

		wl_surface_attach(_wlSurface, _wlBuffer, 0, 0);
		wl_surface_set_buffer_scale(_wlSurface, scale);
		wl_surface_damage_buffer(_wlSurface, 0, 0,_width * scale, _height * scale);
		wl_surface_commit(_wlSurface);
*/

		// wl_surface_attach(_wlSurface, nullptr, 0, 0);
		// wl_surface_set_buffer_scale(_wlSurface, 1.0);
		// wl_surface_damage_buffer(_wlSurface, 0, 0,_width * scale, _height * scale);
		wl_surface_commit(_wlSurface);
		return true;
	}
}
