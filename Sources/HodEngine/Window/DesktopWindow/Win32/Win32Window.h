#pragma once

#include <Windows.h>

#include "HodEngine/Window/DesktopWindow/DesktopWindow.h"

#include <HodEngine/Core/Job/MemberFunctionJob.h>

#include "HodEngine/Core/Event.h"

namespace hod::window
{
	/// @brief 
	class Win32Window : public DesktopWindow
	{
	public:

		Event<HWND, UINT, WPARAM, LPARAM>	OnWinProc;

	public:
											Win32Window();
											~Win32Window() override;

		void								Update() override;

		HWND								GetWindowHandle() const;
		HINSTANCE							GetInstanceHandle() const;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

		void								RunOnWin32Thread(std::function<void()> codeToRun);

	private:

		static LRESULT						WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT								InternalWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		HINSTANCE							_hInstance = nullptr;
		ATOM								_class = 0;
		HWND								_hWnd = nullptr;

		MemberFunctionJob<Win32Window>		_updateJob;

		Thread::Id							_hWndThreadId;

		std::vector<std::function<void()>>	_runOnWin32Thread;
	};
}