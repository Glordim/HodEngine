#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/DisplayManager.hpp"

#undef CreateWindow

#include "HodEngine/Core/StaticArray.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline window
{
	class Window;
	class Cursor;

	class HOD_WINDOW_API DesktopDisplayManager : public DisplayManager
	{
		_SingletonAbstractOverride(DesktopDisplayManager)

		public :

		/// @brief A physical screen, in desktop coordinates (pixels, the primary monitor's top-left corner at the origin).
		struct Monitor
		{
			Vector2 _position = Vector2::Zero;
			Vector2 _size = Vector2::Zero;
			Vector2 _workPosition = Vector2::Zero; // Usable area: the screen minus the taskbar/dock/menu bar
			Vector2 _workSize = Vector2::Zero;
			bool    _primary = false;
		};

		~DesktopDisplayManager() override;

		virtual Window* CreateWindow(bool hidden = false) = 0;
		virtual void DestroyWindow(Window* window) = 0;

		/// @brief The connected monitors, the primary one first. Empty when the platform doesn't report them (yet).
		virtual Vector<Monitor> GetMonitors() const;

		enum class BuiltinCursor
		{
			Arrow,
			Ibeam,
			Wait,
			Cross,
			No,
			Hand,
			SizeWE,
			SizeNS,
			SizeNESW,
			SizeNWSE,

			Count
		};

		Cursor* GetBultinCursor(BuiltinCursor builtinCursor);

	protected:
		virtual Cursor* CreateBuiltinCursor(BuiltinCursor builtinCursor) = 0;

	private:
		StaticArray<Cursor*, static_cast<uint32_t>(BuiltinCursor::Count)> _builtinCursors = {nullptr};
	};
}
