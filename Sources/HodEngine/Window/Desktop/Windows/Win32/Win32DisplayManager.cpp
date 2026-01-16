#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Cursor.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"

#include <HodEngine/Core/OS.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <ole2.h>

#undef CreateWindow

namespace hod::window
{
	constexpr UINT WM_CREATE_WINDOW = WM_USER + 1;
	constexpr UINT WM_DESTROY_WINDOW = WM_USER + 2;

	_SingletonOverrideConstructor(Win32DisplayManager)
	: DesktopDisplayManager()
	{
	}

	/// @brief
	/// @return
	bool Win32DisplayManager::Initialize()
	{
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		OleInitialize(nullptr);

		_hInstance = ::GetModuleHandle(NULL);

		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = &Win32Window::WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = _hInstance;
		windowClass.hIcon = NULL;
		windowClass.hCursor = ::LoadCursor(_hInstance, IDC_ARROW);
		windowClass.hbrBackground = ::CreateSolidBrush(RGB(0, 0, 0));
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = _className;
		windowClass.hIconSm = NULL;
		windowClass.hbrBackground = NULL;

		_class = ::RegisterClassEx(&windowClass);
		if (_class == INVALID_ATOM)
		{
			OUTPUT_ERROR("Win32DisplayManager: Unable to RegisterClass -> {}", OS::GetLastWin32ErrorMessage());
			return false;
		}

		MSG msg = {};
		// Use PeekMessage with PM_NOREMOVE to ensure WindowMessageQueue creation
		PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE);

		_mainThreadId = Thread::GetCurrentThreadId();
		_mainWindow = CreateWindow();
		return true;
	}

	/// @brief
	bool Win32DisplayManager::Run()
	{
		MSG  msg = {};
		BOOL result = 0;

		while ((result = GetMessageA(&msg, nullptr, 0, 0)) > 0)
		{
			if (msg.hwnd == nullptr)
			{
				if (msg.message == WM_CREATE_WINDOW)
				{
					Win32Window* window = reinterpret_cast<Win32Window*>(msg.wParam);
					window->CreateHWND();
				}
				else if (msg.message == WM_DESTROY_WINDOW)
				{
					// todo ?
				}
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (result == -1)
		{
			OUTPUT_ERROR("Win32DisplayManager GetMessageA: {}", OS::GetLastWin32ErrorMessage());
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	void Win32DisplayManager::Update()
	{
		for (Window* window : _windows)
		{
			window->Update();
		}
	}

	/// @brief
	void Win32DisplayManager::Terminate()
	{
		if (_class != INVALID_ATOM)
		{
			::UnregisterClass(_className, _hInstance);
			_class = INVALID_ATOM;
		}

		_hInstance = NULL;

		OleUninitialize();
	}

	/// @brief
	/// @return
	Window* Win32DisplayManager::CreateWindow(bool hidden)
	{
		Win32Window* window = DefaultAllocator::GetInstance().New<Win32Window>(hidden);
		if (_mainThreadId == Thread::GetCurrentThreadId())
		{
			window->CreateHWND();
		}
		else
		{
			if (PostThreadMessageA(_mainThreadId, WM_CREATE_WINDOW, reinterpret_cast<WPARAM>(window), 0) == FALSE)
			{
				OUTPUT_ERROR("CreateWindow PostThreadMessageA: {}", OS::GetLastWin32ErrorMessage());
				DefaultAllocator::GetInstance().Delete(window);
				return nullptr;
			}
		}
		_windows.push_back(window);
		return window;
	}

	/// @brief
	/// @param window
	void Win32DisplayManager::DestroyWindow(Window* window)
	{
		auto it = std::find(_windows.Begin(), _windows.End(), window);
		if (it != _windows.End())
		{
			_windows.Erase(it);
		}
		DefaultAllocator::GetInstance().Delete(window);
	}

	Cursor* Win32DisplayManager::CreateBuiltinCursor(BuiltinCursor builtinCursor)
	{
		static const char* builtinWin32CursorNames[static_cast<uint32_t>(BuiltinCursor::Count)] = {
			IDC_ARROW, IDC_IBEAM, IDC_WAIT, IDC_CROSS, IDC_NO, IDC_HAND, IDC_SIZEWE, IDC_SIZENS, IDC_SIZENESW, IDC_SIZENWSE,
		};

		HCURSOR cursorHandle = LoadCursorA(nullptr, builtinWin32CursorNames[static_cast<uint32_t>(builtinCursor)]);
		return DefaultAllocator::GetInstance().New<Win32Cursor>(cursorHandle);
	}
}
