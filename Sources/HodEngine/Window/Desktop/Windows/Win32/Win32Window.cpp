#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
#include "HodEngine/Window/Surface.hpp"

#include <string>

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/OS.hpp>

#include <cstdlib>

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
		OnWinProc.Emit(_hWnd, msg, wParam, lParam);

		if (msg == WM_SIZE)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			_width = width;
			_height = height;

			Surface* surface = GetSurface();
			if (surface != nullptr)
			{
				surface->Resize(width, height);
			}
		}
		else if (msg == WM_CLOSE)
		{
			_close = true;
			::DestroyWindow(_hWnd);
		}
		else if (msg == WM_DESTROY)
		{
			HWND hwnd = _hWnd;
			_hWnd = nullptr;
			SetWindowLongPtr(nullptr, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			return ::DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return ::DefWindowProc(_hWnd, msg, wParam, lParam);
	}

	/// @brief 
	Win32Window::Win32Window(bool hidden)
		: DesktopWindow()
	{
		_hWndThreadId = Thread::GetCurrentThreadId();

		_hInstance = ::GetModuleHandle(NULL);

		_hWnd = ::CreateWindowEx(
			0,
			Win32DisplayManager::_className,
			"Window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			_width,
			_height,
			NULL,
			NULL,
			_hInstance,
			NULL
		);

		if (_hWnd == NULL)
		{
			OUTPUT_ERROR("DesktopWindow: Unable to CreateWindow -> {}", OS::GetLastWin32ErrorMessage());
			return;
		}

		SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		if (hidden == false)
		{
			::ShowWindow(_hWnd, SW_NORMAL);
		}
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
		if (_hWndThreadId != Thread::GetCurrentThreadId())
		{
			return;
		}

		for (const std::function<void()>& function : _runOnWin32Thread)
		{
			function();
		}
		_runOnWin32Thread.clear();

		::MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

		while (::PeekMessage(&msg, _hWnd, 0, 0, PM_REMOVE) != 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
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

		RunOnWin32Thread(
			[this]()
			{
				SetWindowPos(_hWnd, nullptr, 0, 0, _width, _height, SWP_NOMOVE);
			}
		);
	}

	void Win32Window::CenterToScreen()
	{
		RunOnWin32Thread(
			[this]()
			{
				int x = static_cast<int>(GetSystemMetrics(SM_CXSCREEN) * 0.5f - _width * 0.5f);
				int y = static_cast<int>(GetSystemMetrics(SM_CYSCREEN) * 0.5f - _height * 0.5f);
				SetWindowPos(_hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE);
			}
		);
	}

	void Win32Window::Maximize()
	{
		RunOnWin32Thread(
			[this]()
			{
				ShowWindow(_hWnd, SW_MAXIMIZE);
			}
		);
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
			}
		);
	}

	void Win32Window::RunOnWin32Thread(std::function<void()> codeToRun)
	{
		if (_hWndThreadId == Thread::GetCurrentThreadId())
		{
			codeToRun();
		}
		else
		{
			_runOnWin32Thread.push_back(codeToRun);
		}
	}

	/// @brief 
	/// @return 
	Thread::Id Win32Window::GetMessageLoopThreadId() const
	{
		return _hWndThreadId;
	}
}
