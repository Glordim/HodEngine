#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp"
#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandWindow.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <fcntl.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>

static const char* hodTag = "wlHodTag";

namespace hod::window
{
	_SingletonOverrideConstructor(WaylandDisplayManager)
	: DesktopDisplayManager()
	{
	}

	/// @brief
	/// @return
	bool WaylandDisplayManager::Initialize()
	{
		_wlDisplay = wl_display_connect(nullptr);
		if (_wlDisplay == nullptr)
		{
			OUTPUT_ERROR("WaylandDisplayManager: Can't connect to display ({})", strerror(errno));
			return false;
		}

		// wl_list_init(&seats);
		// wl_list_init(&outputs);

		wl_registry* registry = wl_display_get_registry(_wlDisplay);
		if (registry == nullptr)
		{
			OUTPUT_ERROR("WaylandDisplayManager: Can't get registry");
			return false;
		}

		static const wl_registry_listener registryListener = {
			.global = &WaylandDisplayManager::RegistryHandler,
			.global_remove = &WaylandDisplayManager::RegistryRemover,
		};
		wl_registry_add_listener(registry, &registryListener, this); // todo return value ?

		wl_display_dispatch_pending(_wlDisplay);
		// First roundtrip to receive all registry objects.
		wl_display_roundtrip(_wlDisplay); // todo return value ?
		                                  // Second roundtrip to receive all output events.
		wl_display_roundtrip(_wlDisplay); // todo return value ?

		wl_display_dispatch_pending(_wlDisplay);

		// wl_registry_destroy(registry);

		if (_wlCompositor == nullptr)
		{
			OUTPUT_ERROR("WaylandDisplayManager: Can't find compositor");
			return false;
		}

		wl_display_sync(_wlDisplay);

		// If the compositor doesn't support server side decoration (gnome or other), use libdecor for client side decoration
		if (_zxdgDecorationManager == nullptr)
		{
			static libdecor_interface libDecorInterface = {
				.error = &WaylandDisplayManager::LibDecorErrorHandler,
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

			_libDecorContext = libdecor_new(_wlDisplay, &libDecorInterface);
			if (_libDecorContext == nullptr)
			{
				OUTPUT_ERROR("WaylandDisplayManager: Can't create LibDecor context");
				return false;
			}
		}

		// while (true)
		{
			// wl_display_dispatch(_wlDisplay);
			// libdecor_dispatch(_libDecorContext, -1);
		}

		_mainWindow = CreateWindow();

		return true;
	}

	/// @brief
	void WaylandDisplayManager::Terminate()
	{
		if (_wlDisplay != nullptr)
		{
			wl_display_disconnect(_wlDisplay);
		}
	}

	/// @brief
	/// @param userData
	/// @param registry
	/// @param id
	/// @param
	/// @param version
	void WaylandDisplayManager::RegistryHandler(void* userData, wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
	{
		//OUTPUT_MESSAGE("Got a registry event for {} id {}", interface, id);

		if (std::strcmp(interface, wl_compositor_interface.name) == 0)
		{
			if (version < 4)
			{
				OUTPUT_ERROR("WaylandDisplayManager: compositor version 4+ required but only version {} is available", version);
			}
			else
			{
				WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
				thiz->_wlCompositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 4);
			}
		}
		else if (std::strcmp(interface, wl_shm_interface.name) == 0)
		{
			WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
			thiz->_wlShm = (wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
		}
		else if (std::strcmp(interface, wl_seat_interface.name) == 0)
		{
			if (version < 3)
			{
				OUTPUT_ERROR("WaylandDisplayManager: seat version 3+ required but only version {} is available", version);
			}
			else
			{
				WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
				thiz->_wlSeat = (wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 3);
				static const wl_seat_listener seatListener = {
					.capabilities = &WaylandDisplayManager::SeatCapabilities,
					.name = &WaylandDisplayManager::SeatName,
				};
				wl_seat_add_listener(thiz->_wlSeat, &seatListener, thiz);
			}
		}
		else if (std::strcmp(interface, wl_output_interface.name) == 0)
		{
			if (version < 2)
			{
				OUTPUT_ERROR("WaylandDisplayManager: output version 2+ required but only version {} is available", version);
			}
			else
			{
				wl_output* wlOutput = (wl_output*)wl_registry_bind(registry, id, &wl_output_interface, version);

				WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
				thiz->CreateOutput(wlOutput, id);
			}
		}
		else if (std::strcmp(interface, xdg_wm_base_interface.name) == 0)
		{
			WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
			thiz->_xdgWmBase = (xdg_wm_base*)wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);

			static const xdg_wm_base_listener xdgWmBaseListener = {
				.ping = &WaylandDisplayManager::XdgWmBasePing,
			};

			xdg_wm_base_add_listener(thiz->_xdgWmBase, &xdgWmBaseListener, thiz);
		}
		else if (std::strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
		{
			WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
			thiz->_zxdgDecorationManager = (zxdg_decoration_manager_v1*)wl_registry_bind(registry, id, &zxdg_decoration_manager_v1_interface, 1);
		}
	}

	/// @brief
	/// @param userData
	/// @param registry
	/// @param id
	void WaylandDisplayManager::RegistryRemover(void* userData, wl_registry* /*registry*/, uint32_t id)
	{
		OUTPUT_MESSAGE("Got a registry losing event for %d", id);

		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
		auto                   it = thiz->_outputs.Begin();
		auto                   itEnd = thiz->_outputs.End();
		while (it != itEnd)
		{
			if ((*it)->GetId() == id)
			{
				DefaultAllocator::GetInstance().Delete(*it);
				thiz->_outputs.Erase(it);
				break;
			}
			++it;
		}
	}

	/// @brief
	/// @param userData
	/// @param seat
	/// @param name
	void WaylandDisplayManager::SeatName(void* userData, wl_seat* /*wlSeat*/, const char* name)
	{
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
		thiz->_seatName = name;
	}

	/// @brief
	/// @param userData
	/// @param seat
	/// @param capabilities
	void WaylandDisplayManager::SeatCapabilities(void* userData, wl_seat* /*wlSeat*/, uint32_t capabilities)
	{
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);

		if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && thiz->_wlPointer == nullptr)
		{
			thiz->_wlPointer = wl_seat_get_pointer(thiz->_wlSeat);

			static const wl_pointer_listener pointerListener = {
				.enter = &WaylandDisplayManager::PointerEnter,
				.leave = &WaylandDisplayManager::PointerLeave,
				.motion = &WaylandDisplayManager::PointerMotion,
				.button = &WaylandDisplayManager::PointerButton,
				.axis = &WaylandDisplayManager::PointerAxis,
				.frame = nullptr,
				.axis_source = nullptr,
				.axis_stop = nullptr,
				.axis_discrete = nullptr,
				.axis_value120 = nullptr,
				.axis_relative_direction = nullptr,
			};

			wl_pointer_add_listener(thiz->_wlPointer, &pointerListener, thiz);
			thiz->_pointerScale = 1;
			// init_cursors(seat);
		}
		else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && thiz->_wlPointer != nullptr)
		{
			wl_pointer_release(thiz->_wlPointer);
			thiz->_wlPointer = nullptr;
		}

		if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && thiz->_wlKeyboard == nullptr)
		{
			thiz->_wlKeyboard = wl_seat_get_keyboard(thiz->_wlSeat);
			//thiz->_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

			static const wl_keyboard_listener keyboardListener = {
				.keymap = &WaylandDisplayManager::KeyboardKeymap,
				.enter = &WaylandDisplayManager::KeyboardEnter,
				.leave = &WaylandDisplayManager::KeyboardLeave,
				.key = &WaylandDisplayManager::KeyboardKey,
				.modifiers = &WaylandDisplayManager::KeyboardModifiers,
				.repeat_info = &WaylandDisplayManager::KeyboardRepeatInfo,
			};

			wl_keyboard_add_listener(thiz->_wlKeyboard, &keyboardListener, thiz);
		}
		else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && thiz->_wlKeyboard != nullptr)
		{
			//xkb_context_unref(thiz->_xkbContext);
			wl_keyboard_release(thiz->_wlKeyboard);
			thiz->_wlKeyboard = NULL;
		}
	}

	/// @brief
	/// @param userData
	/// @param wlPointer
	/// @param serial
	/// @param surface
	/// @param surfaceX
	/// @param surfaceY
	void WaylandDisplayManager::PointerEnter(void* userData, wl_pointer* /*wlPointer*/, uint32_t serial, wl_surface* surface, wl_fixed_t surfaceX, wl_fixed_t surfaceY)
	{
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
		thiz->_pointerFocus = surface;
		thiz->_serial = serial;

		/*
		if (surface != window->wl_surface)
		{
		    return;
		}
		*/

		// set_cursor(seat);

		thiz->_pointerX = surfaceX;
		thiz->_pointerY = surfaceY;
	}

	/// @brief
	/// @param userData
	/// @param wlPointer
	/// @param serial
	/// @param surface
	void WaylandDisplayManager::PointerLeave(void* userData, wl_pointer* /*wlPointer*/, uint32_t /*serial*/, wl_surface* surface)
	{
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
		if (thiz->_pointerFocus == surface)
		{
			thiz->_pointerFocus = nullptr;
		}
	}

	/// @brief
	/// @param userData
	/// @param wlPointer
	/// @param time
	/// @param surfaceX
	/// @param surfaceY
	void WaylandDisplayManager::PointerMotion(void* userData, wl_pointer* /*wlPointer*/, uint32_t /*time*/, wl_fixed_t surfaceX, wl_fixed_t surfaceY)
	{
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);

		if (thiz->_surfaceToWindowMap.contains(thiz->_pointerFocus) == true)
		{
			WaylandWindow* waylandWindow = thiz->_surfaceToWindowMap[thiz->_pointerFocus];
			waylandWindow->SetMousePosition(Vector2(wl_fixed_to_double(surfaceX), wl_fixed_to_double(surfaceY)));
		}
	}

	/// @brief
	/// @param userData
	/// @param wlPointer
	/// @param serial
	/// @param time
	/// @param button
	/// @param state
	void WaylandDisplayManager::PointerButton(void* userData, wl_pointer* /*wlPointer*/, uint32_t /*serial*/, uint32_t /*time*/, uint32_t button, uint32_t state)
	{
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);

		if (thiz->_surfaceToWindowMap.contains(thiz->_pointerFocus) == true)
		{
			WaylandWindow* waylandWindow = thiz->_surfaceToWindowMap[thiz->_pointerFocus];

			MouseButton mouseButton;
			if (button == BTN_LEFT)
			{
				mouseButton = MouseButton::Left;
			}
			else if (button == BTN_MIDDLE)
			{
				mouseButton = MouseButton::Middle;
			}
			else if (button == BTN_RIGHT)
			{
				mouseButton = MouseButton::Right;
			}
			else
			{
				return; // todo ?
			}

			if (state == WL_POINTER_BUTTON_STATE_PRESSED)
			{
				waylandWindow->EmitMouseButtonPressed(mouseButton);
			}
			else
			{
				waylandWindow->EmitMouseButtonReleased(mouseButton);
			}
		}
		/*
		if (seat->_pointerFocus != window->wl_surface)
		{
		    return;
		}

		seat->_serial = serial;

		//if (window->popup && state == WL_POINTER_BUTTON_STATE_PRESSED)
		//{
		//   close_popup(window);
		///}

		if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
		{
		    libdecor_frame_move(window->frame, seat->_wlSeat, serial);
		}
		else if (button == BTN_MIDDLE && state == WL_POINTER_BUTTON_STATE_PRESSED)
		{
		    libdecor_frame_show_window_menu(window->frame, seat->_wlSeat, serial, wl_fixed_to_int(seat->_pointerX), wl_fixed_to_int(seat->_pointerY));
		}
		else if (button == BTN_RIGHT && state == WL_POINTER_BUTTON_STATE_PRESSED)
		{
		    //if (!window->popup)
		    //{
		    //    open_popup(seat);
		    //}
		}
		*/
	}

	/// @brief
	/// @param userData
	/// @param wlPointer
	/// @param time
	/// @param axis
	/// @param value
	void WaylandDisplayManager::PointerAxis(void* /*userData*/, wl_pointer* /*wlPointer*/, uint32_t /*time*/, uint32_t /*axis*/, wl_fixed_t /*value*/) {}

	/// @brief
	/// @param userData
	/// @param wl_keyboard
	/// @param format
	/// @param fd
	/// @param Size
	void WaylandDisplayManager::KeyboardKeymap(void* /*userData*/, wl_keyboard* /*wl_keyboard*/, uint32_t format, int32_t fd, uint32_t Size)
	{
		//WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);

		if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
		{
			close(fd);
			return;
		}

		char* mapStr = (char*)mmap(nullptr, Size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (mapStr == MAP_FAILED)
		{
			close(fd);
			OUTPUT_ERROR("Keymap mmap failed: {}", strerror(errno));
			return;
		}

		//xkb_keymap* keymap = xkb_keymap_new_from_string(thiz->_xkbContext, mapStr, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
		munmap(mapStr, Size);
		close(fd);

		//if (keymap == nullptr)
		{
			return;
		}

		//thiz->_xkbState = xkb_state_new(keymap);

		//xkb_keymap_unref(keymap);
	}

	/// @brief
	/// @param userData
	/// @param wl_keyboard
	/// @param serial
	/// @param surface
	/// @param keys
	void WaylandDisplayManager::KeyboardEnter(void* /*userData*/, wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, wl_surface* /*surface*/, wl_array* /*keys*/) {}

	/// @brief
	/// @param userData
	/// @param wl_keyboard
	/// @param serial
	/// @param surface
	void WaylandDisplayManager::KeyboardLeave(void* /*userData*/, wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, wl_surface* /*surface*/) {}

	/// @brief
	/// @param userData
	/// @param wl_keyboard
	/// @param serial
	/// @param time
	/// @param key
	/// @param state
	void WaylandDisplayManager::KeyboardKey(void* /*userData*/, wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, uint32_t /*time*/, uint32_t /*key*/, uint32_t /*state*/)
	{
		/*
		WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);
		
		if (state & WL_KEYBOARD_KEY_STATE_PRESSED)
		{
		    const xkb_keysym_t* syms;
		    if (xkb_state_key_get_syms(thiz->_xkbState, key + 8, &syms) != 1)
		    {
		        return;
		    }

		    switch (syms[0])
		    {
		    case XKB_KEY_Escape:
		        printf("close\n");
		        libdecor_frame_close(window->frame);
		        break;
		    case XKB_KEY_1: // toggle resizability
		        if (libdecor_frame_has_capability(
		                window->frame, LIBDECOR_ACTION_RESIZE)) {
		            printf("set fixed-Size\n");
		            libdecor_frame_unset_capabilities(window->frame,
		                        LIBDECOR_ACTION_RESIZE);
		        } else {
		            printf("set resizeable\n");
		            libdecor_frame_set_capabilities(window->frame,
		                        LIBDECOR_ACTION_RESIZE);
		        }
		        break;
		    case XKB_KEY_2: // maximize
		        printf("maximize\n");
		        libdecor_frame_set_maximized(window->frame);
		        break;
		    case XKB_KEY_3: // un-maximize / restore
		        printf("un-maximize\n");
		        libdecor_frame_unset_maximized(window->frame);
		        break;
		    case XKB_KEY_4: // fullscreen
		        printf("fullscreen\n");
		        libdecor_frame_set_fullscreen(window->frame, NULL);
		        break;
		    case XKB_KEY_5: // un-fullscreen / restore
		        printf("un-fullscreen\n");
		        libdecor_frame_unset_fullscreen(window->frame);
		        break;
		    case XKB_KEY_minus:
		    case XKB_KEY_plus:
		        {
		            const int dd = (syms[0] == XKB_KEY_minus ? -1 : +1) * chk/2;
		            printf("Resize to: %i x %i\n",
		                window->configured_width + dd,
		                window->configured_height + dd);
		            Resize(window,
		                window->configured_width + dd,
		                window->configured_height + dd);
		        }
		        break;
		    case XKB_KEY_v: // VGA: 640x480
		        printf("set VGA resolution: 640x480\n");
		        Resize(window, 640, 480);
		        break;
		    case XKB_KEY_s: // SVGA: 800x600
		        printf("set SVGA resolution: 800x600\n");
		        Resize(window, 800, 600);
		        break;
		    case XKB_KEY_x: // XVGA: 1024x768
		        printf("set XVGA resolution: 1024x768\n");
		        Resize(window, 1024, 768);
		        break;
		    case XKB_KEY_t:
		        libdecor_frame_set_title(window->frame, titles[window->title_index]);
		        window->title_index = (window->title_index + 1) % N_TITLES;
		        break;
		    case XKB_KEY_h: // toggle decorations
		        libdecor_frame_set_visibility(
		                window->frame,
		                !libdecor_frame_is_visible(window->frame));
		        printf("decorations %s\n",
		            libdecor_frame_is_visible(window->frame) ?
		                "visible" : "hidden");
		        break;
		    }
		}
		*/
	}

	/// @brief
	/// @param userData
	/// @param wl_keyboard
	/// @param serial
	/// @param modsDepressed
	/// @param modsLatched
	/// @param modsLocked
	/// @param group
	void WaylandDisplayManager::KeyboardModifiers(void* /*userData*/, wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, uint32_t /*modsDepressed*/, uint32_t /*modsLatched*/, uint32_t /*modsLocked*/,
	                                              uint32_t /*group*/)
	{
		//WaylandDisplayManager* thiz = static_cast<WaylandDisplayManager*>(userData);

		//xkb_state_update_mask(thiz->_xkbState, modsDepressed, modsLatched, modsLocked, 0, 0, group);
	}

	/// @brief
	/// @param userData
	/// @param wlKeyboard
	/// @param rate
	/// @param delay
	void WaylandDisplayManager::KeyboardRepeatInfo(void* /*userData*/, wl_keyboard* /*wlKeyboard*/, int32_t /*rate*/, int32_t /*delay*/) {}

	/// @brief
	/// @param context
	/// @param error
	/// @param message
	void WaylandDisplayManager::LibDecorErrorHandler(libdecor* /*context*/, libdecor_error error, const char* message)
	{
		OUTPUT_ERROR("LibDecor: Caught error ({}): {}", (int)error, message);
	}

	/// @brief
	/// @param data
	/// @param xdg_wm_base
	/// @param serial
	void WaylandDisplayManager::XdgWmBasePing(void* /*userData*/, xdg_wm_base* xdg_wm_base, uint32_t serial)
	{
		xdg_wm_base_pong(xdg_wm_base, serial);
	}

	/// @brief
	void WaylandDisplayManager::Update()
	{
		if (_libDecorContext != nullptr)
		{
			libdecor_dispatch(_libDecorContext, 0);
		}
		else
		{
			wl_display_roundtrip(_wlDisplay);
		}
	}

	/// @brief
	/// @return
	Window* WaylandDisplayManager::CreateWindow(bool /*hidden*/)
	{
		WaylandWindow* window = DefaultAllocator::GetInstance().New<WaylandWindow>(); // todo use hidden

		_surfaceToWindowMap[window->GetWaylandSurface()] = window;

		return window;
	}

	/// @brief
	/// @param window
	void WaylandDisplayManager::DestroyWindow(Window* /*window*/) {}

	/// @brief
	/// @return
	wl_display* WaylandDisplayManager::GetDisplay() const
	{
		return _wlDisplay;
	}

	/// @brief
	/// @return
	wl_compositor* WaylandDisplayManager::GetCompositor() const
	{
		return _wlCompositor;
	}

	/// @brief
	/// @return
	wl_shm* WaylandDisplayManager::GetShm() const
	{
		return _wlShm;
	}

	/// @brief
	/// @return
	libdecor* WaylandDisplayManager::GetLibDecorContext() const
	{
		return _libDecorContext;
	}

	/// @brief
	/// @return
	xdg_wm_base* WaylandDisplayManager::GetXdgWmBase() const
	{
		return _xdgWmBase;
	}

	/// @brief
	/// @param wlOutput
	/// @return
	WaylandDisplayManager::Output& WaylandDisplayManager::CreateOutput(wl_output* wlOutput, uint32_t id)
	{
		_outputs.push_back(DefaultAllocator::GetInstance().New<Output>(wlOutput, id));
		return *_outputs.Back();
	}

	/// @brief
	/// @param wlOutput
	WaylandDisplayManager::Output::Output(wl_output* wlOutput, uint32_t id)
	: _wlOutput(wlOutput)
	, _id(id)
	{
		static const wl_output_listener outputListener = {
			.geometry = &WaylandDisplayManager::Output::OutputGeometry,
			.mode = &WaylandDisplayManager::Output::OutputMode,
			.done = &WaylandDisplayManager::Output::OutputDone,
			.scale = &WaylandDisplayManager::Output::OutputScale,
			.name = &WaylandDisplayManager::Output::OutputName,
			.description = &WaylandDisplayManager::Output::OutputDescription,
		};

		wl_output_add_listener(_wlOutput, &outputListener, this);
		wl_proxy_set_tag((wl_proxy*)_wlOutput, &hodTag);
		// wl_list_insert(&outputs, &output->link);
	}

	/// @brief
	WaylandDisplayManager::Output::~Output()
	{
		wl_output_destroy(_wlOutput);
	}

	/// @brief
	/// @return
	uint32_t WaylandDisplayManager::Output::GetId() const
	{
		return _id;
	}

	/// @brief
	/// @param userData
	/// @param wl_output
	/// @param x
	/// @param y
	/// @param physicalWidth
	/// @param physicalHeight
	/// @param subpixel
	/// @param make
	/// @param model
	/// @param transform
	void WaylandDisplayManager::Output::OutputGeometry(void* /*userData*/, wl_output* /*wl_output*/, int32_t /*x*/, int32_t /*y*/, int32_t /*physicalWidth*/, int32_t /*physicalHeight*/, int32_t /*subpixel*/,
	                                                   const char* /*make*/, const char* /*model*/, int32_t /*transform*/)
	{
	}

	/// @brief
	/// @param userData
	/// @param wl_output
	/// @param flags
	/// @param width
	/// @param height
	/// @param refresh
	void WaylandDisplayManager::Output::OutputMode(void* /*userData*/, wl_output* /*wl_output*/, uint32_t /*flags*/, int32_t /*width*/, int32_t /*height*/, int32_t /*refresh*/) {}

	/// @brief
	/// @param userData
	/// @param wl_output
	void WaylandDisplayManager::Output::OutputDone(void* /*userData*/, wl_output* /*wl_output*/)
	{
		/*
		struct output *output = data;
		struct seat *seat;

		if (window) {
		    if (output->scale != window->scale)
		        update_scale(window);
		}

		wl_list_for_each(seat, &seats, link) {
		    try_update_cursor(seat);
		}
		*/
	}

	/// @brief
	/// @param userData
	/// @param wl_output
	/// @param factor
	void WaylandDisplayManager::Output::OutputScale(void* userData, wl_output* /*wl_output*/, int32_t factor)
	{
		Output* thiz = (Output*)userData;
		thiz->_scale = factor;
	}

	/// @brief
	/// @param userData
	/// @param wl_output
	/// @param name
	void WaylandDisplayManager::Output::OutputName(void* /*userData*/, wl_output* /*wl_output*/, const char* /*name*/) {}

	/// @brief
	/// @param userData
	/// @param wl_output
	/// @param description
	void WaylandDisplayManager::Output::OutputDescription(void* /*userData*/, wl_output* /*wl_output*/, const char* /*description*/) {}
}
