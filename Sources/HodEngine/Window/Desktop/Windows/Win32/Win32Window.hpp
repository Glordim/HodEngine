#pragma once
#include "HodEngine/Window/Export.hpp"

#if defined(PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/Event.hpp"
#include <HodEngine/Core/Job/Thread.hpp>

namespace hod::window
{
	/// @brief 
	class HOD_WINDOW_API Win32Window : public DesktopWindow
	{
	public:

		Event<HWND, UINT, WPARAM, LPARAM>	OnWinProc;
		static LRESULT						WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
											Win32Window(bool hidden = false);
											~Win32Window() override;

		void								Update() override;

		HWND								GetWindowHandle() const;
		HINSTANCE							GetInstanceHandle() const;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

		void								SetVisible(bool visible) override;

		void								RunOnWin32Thread(std::function<void()> codeToRun);
		Thread::Id							GetMessageLoopThreadId() const;

	private:

		LRESULT								InternalWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		HINSTANCE							_hInstance = nullptr;
		HWND								_hWnd = nullptr;

		Thread::Id							_hWndThreadId;

		std::vector<std::function<void()>>	_runOnWin32Thread;
	};
}

#endif
