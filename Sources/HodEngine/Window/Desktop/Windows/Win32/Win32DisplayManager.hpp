#pragma once
#include "HodEngine/Window/Export.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CreateWindow

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"
#include <HodEngine/Core/Job/Thread.hpp>

namespace hod::window
{
	class HOD_WINDOW_API Win32DisplayManager : public DesktopDisplayManager
	{
		_SingletonOverride(Win32DisplayManager)

	public:
		static constexpr const char* _className = "HodWin32Window";

	public:
		bool Initialize() override;
		void Update() override;
		bool Run() override;
		void Terminate() override;

		Window* CreateWindow(bool hidden = false) override;
		void    DestroyWindow(Window* window) override;

	protected:
		Cursor* CreateBuiltinCursor(BuiltinCursor builtinCursor) override;

	private:
		HINSTANCE _hInstance = NULL;
		ATOM      _class = INVALID_ATOM;

		Vector<Window*> _windows;
		Thread::Id      _mainThreadId;
	};
}
