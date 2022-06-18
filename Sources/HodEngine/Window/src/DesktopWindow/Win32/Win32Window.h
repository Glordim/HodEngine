#pragma once

#include <Windows.h>

#include "HodEngine/Window/src/DesktopWindow/DesktopWindow.h"

#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>

#include "HodEngine/Core/Src/Signal.h"

namespace hod::window
{
	/// @brief 
	class Win32Window : public DesktopWindow
	{
		using WinProcEvent = CORE::Signal<HWND, UINT, WPARAM, LPARAM>;

	public:
											Win32Window();
											~Win32Window() override;

		void								Update() override;

		HWND								GetWindowHandle() const;
		HINSTANCE							GetInstanceHandle() const;

		WinProcEvent&						GetWinProcEvent();

	private:

		static LRESULT						WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT								InternalWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		HINSTANCE							_hInstance = NULL;
		ATOM								_class = 0;
		HWND								_hWnd = NULL;

		MemberFunctionJob<Win32Window>		_updateJob;

		Thread::Id							_hWndThreadId;

		WinProcEvent						_winProcEvent;
	};
}
