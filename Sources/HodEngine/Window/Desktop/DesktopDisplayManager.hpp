#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/DisplayManager.hpp"

#undef CreateWindow

#include <array>

namespace hod::window
{
	class Window;
	class Cursor;

	class HOD_WINDOW_API DesktopDisplayManager : public DisplayManager
	{
	_SingletonAbstractOverride(DesktopDisplayManager)

		public :

		virtual Window* CreateWindow(bool hidden = false) = 0;
		virtual void DestroyWindow(Window* window) = 0;

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
		std::array<Cursor*, static_cast<uint32_t>(BuiltinCursor::Count)> _builtinCursors = {nullptr};
	};
}
