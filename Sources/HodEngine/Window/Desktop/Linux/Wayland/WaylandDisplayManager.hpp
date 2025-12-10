#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

#include <wayland-client.h>
// #include <xkbcommon/xkbcommon.h>

#include <libdecor-0/libdecor.h>

#include "HodEngine/Core/String.hpp"
#include <map>

struct xdg_wm_base;
struct zxdg_decoration_manager_v1;

namespace hod::window
{
	class WaylandWindow;

	class HOD_WINDOW_API WaylandDisplayManager : public DesktopDisplayManager
	{
		_SingletonOverride(WaylandDisplayManager)

	public:
		bool Initialize() override;
		void Update() override;
		bool Run() override;
		void Terminate() override;

		Window* CreateWindow(bool hidden = false) override;
		void    DestroyWindow(Window* window) override;

		wl_display*    GetDisplay() const;
		wl_compositor* GetCompositor() const;
		wl_shm*        GetShm() const;
		libdecor*      GetLibDecorContext() const;
		xdg_wm_base*   GetXdgWmBase() const;

	protected:
		Cursor* CreateBuiltinCursor(BuiltinCursor builtinCursor) override;

	private:
		class Output final
		{
		public:
			Output(wl_output* output, uint32_t id);
			~Output();

			uint32_t GetId() const;

		private:
			static void OutputGeometry(void* userData, wl_output* wl_output, int32_t x, int32_t y, int32_t physicalWidth, int32_t physicalHeight, int32_t subpixel,
			                           const char* make, const char* model, int32_t transform);
			static void OutputMode(void* userData, wl_output* wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
			static void OutputDone(void* userData, wl_output* wl_output);
			static void OutputScale(void* userData, wl_output* wl_output, int32_t factor);
			static void OutputName(void* userData, wl_output* wl_output, const char* name);
			static void OutputDescription(void* userData, wl_output* wl_output, const char* description);

		private:
			wl_output* _wlOutput = nullptr;
			uint32_t   _id = 0;
			int32_t    _scale = 1;
		};

	private:
		Output& CreateOutput(wl_output* wlOutput, uint32_t id);

	private:
		static void HandleGlobal(void* userData, wl_registry* registry, uint32_t id, const char* interface, uint32_t version);
		void		HandleGlobal(wl_registry* registry, uint32_t id, const char* interface, uint32_t version);

		static void HandleGlobalRemove(void* userData, wl_registry* registry, uint32_t id);
		void		HandleGlobalRemove(wl_registry* registry, uint32_t id);

		static void SeatName(void* userData, wl_seat* seat, const char* name);
		static void SeatCapabilities(void* userData, wl_seat* seat, uint32_t capabilities);

		static void PointerEnter(void* userData, wl_pointer* wlPointer, uint32_t serial, wl_surface* surface, wl_fixed_t surfaceX, wl_fixed_t surfaceY);
		static void PointerLeave(void* userData, wl_pointer* wlPointer, uint32_t serial, wl_surface* surface);
		static void PointerMotion(void* userData, wl_pointer* wlPointer, uint32_t time, wl_fixed_t surfaceX, wl_fixed_t surfaceY);
		static void PointerButton(void* userData, wl_pointer* wlPointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
		static void PointerAxis(void* userData, wl_pointer* wlPointer, uint32_t time, uint32_t axis, wl_fixed_t value);

		static void KeyboardKeymap(void* userData, wl_keyboard* wl_keyboard, uint32_t format, int32_t fd, uint32_t size);
		static void KeyboardEnter(void* userData, wl_keyboard* wl_keyboard, uint32_t serial, wl_surface* surface, wl_array* keys);
		static void KeyboardLeave(void* userData, wl_keyboard* wl_keyboard, uint32_t serial, wl_surface* surface);
		static void KeyboardKey(void* userData, wl_keyboard* wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
		static void KeyboardModifiers(void* userData, wl_keyboard* wl_keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group);
		static void KeyboardRepeatInfo(void* userData, wl_keyboard* wlKeyboard, int32_t rate, int32_t delay);

		static void LibDecorErrorHandler(libdecor* context, libdecor_error error, const char* message);

		static void XdgWmBasePing(void* userData, xdg_wm_base* xdg_wm_base, uint32_t serial);

	private:
		wl_display*    _wlDisplay = nullptr;
		wl_compositor* _wlCompositor = nullptr;

		libdecor*                   _libDecorContext = nullptr;
		zxdg_decoration_manager_v1* _zxdgDecorationManager = nullptr;
		xdg_wm_base*                _xdgWmBase = nullptr;

		Vector<Output*> _outputs;

		wl_seat* _wlSeat = nullptr;
		String   _seatName;

		wl_keyboard* _wlKeyboard = nullptr;
		wl_pointer*  _wlPointer = nullptr;
		// wl_list         _link;
		// wl_list         _pointerOutputs;
		// wl_cursor_theme* _cursorTheme;
		// wl_cursor* _leftPtrCursor;
		wl_surface* _cursorSurface = nullptr;
		wl_surface* _pointerFocus = nullptr;
		int         _pointerScale;
		uint32_t    _serial;
		wl_fixed_t  _pointerX;
		wl_fixed_t  _pointerY;

		// xkb_context*    _xkbContext = nullptr;
		// xkb_state*      _xkbState = nullptr;

		std::map<wl_surface*, WaylandWindow*> _surfaceToWindowMap;
	};
}
