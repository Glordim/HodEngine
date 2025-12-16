#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Cursor.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"

#include "HodEngine/Core/String.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/OS.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Profiler.hpp>

#include <cstdlib>

#include <windowsx.h>

namespace hod::window
{
	/// @brief
	/// @param hWnd
	/// @param msg
	/// @param wParam
	/// @param lParam
	/// @return
	LRESULT Win32Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Win32Window* win32Window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (win32Window == nullptr)
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return win32Window->InternalWindowProc(msg, wParam, lParam);
	}

	/// @brief
	/// @param msg
	/// @param wParam
	/// @param lParam
	/// @return
	LRESULT Win32Window::InternalWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		PROFILER_SCOPED_EVENT_WITH_PARAM("Win32Window::InternalWindowProc", std::format("msg = {}", msg).c_str())

		if (msg == WM_SETCURSOR)
		{
			if (LOWORD(lParam) == HTCLIENT)
			{
				::SetCursor(static_cast<Win32Cursor*>(GetCursor())->GetCursorHandle());
				return 1;
			}
			return ::DefWindowProc(_hWnd, msg, wParam, lParam);
		}
		else if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
		{
			UINT scancode = (lParam >> 16) & 0xFF;
			scancode |= ((lParam >> 24) & 1) << 8;
			EmitKeyPressed(WindowsScanCodeToScanCode(scancode), WindowsVirtualKeyToKey(wParam));
			return 0;
		}
		else if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
		{
			UINT scancode = (lParam >> 16) & 0xFF;
			scancode |= ((lParam >> 24) & 1) << 8;
			EmitKeyReleased(WindowsScanCodeToScanCode(scancode), WindowsVirtualKeyToKey(wParam));
			return 0;
		}
		else if (msg == WM_CHAR)
		{
			EmitChar(wParam);
			return 0;
		}
		else if (msg == WM_LBUTTONDOWN)
		{
			EmitMouseButtonPressed(MouseButton::Left);
			return 0;
		}
		else if (msg == WM_LBUTTONUP)
		{
			EmitMouseButtonReleased(MouseButton::Left);
			return 0;
		}
		else if (msg == WM_RBUTTONDOWN)
		{
			EmitMouseButtonPressed(MouseButton::Right);
			return 0;
		}
		else if (msg == WM_RBUTTONUP)
		{
			EmitMouseButtonReleased(MouseButton::Right);
			return 0;
		}
		else if (msg == WM_MBUTTONDOWN)
		{
			EmitMouseButtonPressed(MouseButton::Middle);
			return 0;
		}
		else if (msg == WM_MBUTTONUP)
		{
			EmitMouseButtonReleased(MouseButton::Middle);
			return 0;
		}
		else if (msg == WM_XBUTTONDOWN)
		{
			WORD xbtn = HIWORD(wParam);
			if (xbtn == XBUTTON1)
			{
				EmitMouseButtonPressed(MouseButton::Backward);
			}
			else
			{
				EmitMouseButtonPressed(MouseButton::Forward);
			}
			return 0;
		}
		else if (msg == WM_XBUTTONUP)
		{
			WORD xbtn = HIWORD(wParam);
			if (xbtn == XBUTTON1)
			{
				EmitMouseButtonReleased(MouseButton::Backward);
			}
			else
			{
				EmitMouseButtonReleased(MouseButton::Forward);
			}
			return 0;
		}
		else if (msg == WM_MOUSEMOVE)
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			SetMousePosition(Vector2(x, y));

			EmitMouseMoved(x, y);
			return 0;
		}
		else if (msg == WM_MOUSEWHEEL)
		{
			int   delta = GET_WHEEL_DELTA_WPARAM(wParam);
			float scroll = (float)delta / (float)WHEEL_DELTA;
			EmitMouseScroll(scroll);
			return 0;
		}
		else if (msg == WM_MOUSEHWHEEL)
		{
			int   delta = GET_WHEEL_DELTA_WPARAM(wParam);
			float scroll = (float)delta / (float)WHEEL_DELTA;
			EmitMouseHorizontalScroll(scroll);
			return 0;
		}
		else if (msg == WM_SIZE)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			ResizeInternal(width, height);
			return 0;
		}
		else if (msg == WM_CLOSE)
		{
			_close = true;
			::DestroyWindow(_hWnd);
			return 0;
		}
		else if (msg == WM_DESTROY)
		{
			HWND hwnd = _hWnd;
			_hWnd = nullptr;
			SetWindowLongPtr(nullptr, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			if (DisplayManager::GetInstance()->GetMainWindow() == this)
			{
				PostQuitMessage(0);
			}
			return ::DefWindowProc(hwnd, msg, wParam, lParam); // todo ?
		}
		else if (msg == WM_USER + 3)
		{
			_runOnWin32ThreadMutex.lock();
			for (const std::function<void()>& function : _runOnWin32Thread)
			{
				function();
			}
			_runOnWin32Thread.Clear();
			_runOnWin32ThreadMutex.unlock();
			return 0;
		}

		return ::DefWindowProc(_hWnd, msg, wParam, lParam);
	}

	/// @brief
	Win32Window::Win32Window(bool /* hidden*/)
	: DesktopWindow()
	{
		_hWndThreadId = Thread::GetCurrentThreadId();

		_hInstance = ::GetModuleHandle(NULL);

		/*
		if (hidden == false)
		{
		    ::ShowWindow(_hWnd, SW_NORMAL);
		}
		    */
	}

	/// @brief
	Win32Window::~Win32Window()
	{
		if (_hWnd != NULL)
		{
			::DestroyWindow(_hWnd);
		}
	}

	/// @brief
	void Win32Window::Update()
	{
		DesktopWindow::Update();

		/*
		if (_hWndThreadId != Thread::GetCurrentThreadId())
		{
		    return;
		}

		for (const std::function<void()>& function : _runOnWin32Thread)
		{
		    function();
		}
		_runOnWin32Thread.Clear();
		*/
		/*
		::MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

		while (::PeekMessage(&msg, _hWnd, 0, 0, PM_REMOVE) != 0)
		{
		    ::TranslateMessage(&msg);
		    ::DispatchMessage(&msg);
		}
		*/
	}

	bool Win32Window::CreateHWND()
	{
		Assert(_hWnd == nullptr);
		_hWnd = CreateWindowExA(0, Win32DisplayManager::_className, "Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, _width, _height, nullptr, nullptr, _hInstance,
		                        nullptr);

		if (_hWnd == nullptr)
		{
			OUTPUT_ERROR("DesktopWindow: Unable to CreateWindow -> {}", OS::GetLastWin32ErrorMessage());
			return false;
		}
		SetWindowLongPtrA(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(_hWnd, SW_NORMAL);

		return true;
	}

	/// @brief
	/// @return
	HWND Win32Window::GetWindowHandle() const
	{
		return _hWnd;
	}

	/// @brief
	/// @return
	HINSTANCE Win32Window::GetInstanceHandle() const
	{
		return _hInstance;
	}

	void Win32Window::SetSize(uint16_t width, uint16_t height)
	{
		_width = width;
		_height = height;

		RunOnWin32Thread([this]() { SetWindowPos(_hWnd, nullptr, 0, 0, _width, _height, SWP_NOMOVE); });
	}

	void Win32Window::CenterToScreen()
	{
		RunOnWin32Thread(
			[this]()
			{
				int x = static_cast<int>(GetSystemMetrics(SM_CXSCREEN) * 0.5f - _width * 0.5f);
				int y = static_cast<int>(GetSystemMetrics(SM_CYSCREEN) * 0.5f - _height * 0.5f);
				SetWindowPos(_hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE);
			});
	}

	void Win32Window::Maximize()
	{
		RunOnWin32Thread([this]() { ShowWindow(_hWnd, SW_MAXIMIZE); });
	}

	void Win32Window::SetVisible(bool visible)
	{
		RunOnWin32Thread(
			[this, visible]()
			{
				if (visible)
				{
					ShowWindow(_hWnd, SW_NORMAL);
				}
				else
				{
					ShowWindow(_hWnd, SW_HIDE);
				}
			});
	}

	void Win32Window::RunOnWin32Thread(std::function<void()> codeToRun)
	{
		if (_hWndThreadId == Thread::GetCurrentThreadId())
		{
			codeToRun();
		}
		else
		{
			_runOnWin32ThreadMutex.lock();
			if (_runOnWin32Thread.Empty())
			{
				if (PostMessageA(_hWnd, WM_USER + 3, 0, 0) == FALSE)
				{
					OUTPUT_ERROR("RunOnWin32Thread PostMessageA: {}", OS::GetLastWin32ErrorMessage());
				}
			}
			_runOnWin32Thread.push_back(codeToRun);
			_runOnWin32ThreadMutex.unlock();
		}
	}

	/// @brief
	/// @return
	Thread::Id Win32Window::GetMessageLoopThreadId() const
	{
		return _hWndThreadId;
	}
}
