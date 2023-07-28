#if defined(PLATFORM_LINUX)

#include "HodEngine/Window/DesktopWindow/Wayland/WaylandWindow.h"
#include "HodEngine/Core/Output.h"

#include <cstring>

namespace hod::window
{
	static struct wl_display* _display = nullptr;
	static struct wl_compositor* _compositor = nullptr;
	static struct wl_shell* _shell = nullptr;

	static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
	{
		OUTPUT_MESSAGE("Got a registry event for %s id %d", interface, id);
		if (std::strcmp(interface, "wl_compositor") == 0)
		{
			_compositor = (struct wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
		}
		else if (std::strcmp(interface, "wl_shell") == 0)
		{
        	_shell = (struct wl_shell*)wl_registry_bind(registry, id, &wl_shell_interface, 1);
    	}
	}

	static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
	{
		OUTPUT_MESSAGE("Got a registry losing event for %d", id);
	}

	/// @brief 
	WaylandWindow::WaylandWindow()
	{
		_display = wl_display_connect(nullptr);
		if (_display == nullptr)
		{
			OUTPUT_ERROR("Can't connect to display");
			return;
		}

		struct wl_registry* registry = wl_display_get_registry(_display);

		static const struct wl_registry_listener registry_listener = {
			global_registry_handler,
			global_registry_remover
		};

		wl_registry_add_listener(registry, &registry_listener, nullptr);

		wl_display_dispatch(_display);
		wl_display_roundtrip(_display);

		if (_compositor == nullptr)
		{
			OUTPUT_ERROR("Can't find compositor");
			return;
		}

		_surface = wl_compositor_create_surface(_compositor);
		if (_surface == nullptr)
		{
			OUTPUT_ERROR("Can't create surface");
			return;
		}

		if (_shell == nullptr)
		{
			OUTPUT_ERROR("Haven't got a Wayland shell");
			return;
		}

		_shellSurface = wl_shell_get_shell_surface(_shell, _surface);
		if (_shellSurface == nullptr)
		{
			OUTPUT_ERROR("Can't create shell surface\n");
			return;
		}

		wl_shell_surface_set_toplevel(_shellSurface);
	}

	/// @brief 
	WaylandWindow::~WaylandWindow()
	{
		if (_display != nullptr)
		{
			wl_display_disconnect(_display);
		}
	}

	/// @brief 
	void WaylandWindow::Update()
	{

	}

	/// @brief 
	struct wl_display* WaylandWindow::GetWaylandDisplay() const
	{
		return _display;
	}

	/// @brief 
	struct wl_surface* WaylandWindow::GetWaylandSurface() const
	{
		return _surface;
	}

	/// @brief 
	/// @param width 
	/// @param height 
	void WaylandWindow::SetSize(uint16_t width, uint16_t height)
	{

	}
	
	/// @brief 
	void WaylandWindow::CenterToScreen()
	{

	}

	/// @brief 
	void WaylandWindow::Maximize()
	{

	}
}

#endif
