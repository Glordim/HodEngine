#include "Win32Window.h"

#include <string>

#include <HodEngine/Core/Output.h>
#include <HodEngine/Core/Job/JobQueue.h>
#include <HodEngine/Core/Frame/FrameSequencer.h>

//#include <Renderer/RHI/Context.h>

#include <cstdlib>

using namespace hod::renderer;

namespace hod::window
{
	constexpr const char* className = "DesktopWindow";

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
			//_context->Resize(width, height);
		}
		else if (msg == WM_CLOSE)
		{
			::DestroyWindow(_hWnd);
			_hWnd = nullptr;
			exit(0);
		}

		return ::DefWindowProc(_hWnd, msg, wParam, lParam);
	}

	/// @brief 
	/// @return 
	std::string GetLastErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		std::string message = (char*)lpMsgBuf;

		::LocalFree(lpMsgBuf);

		return message;
	}

	/// @brief 
	Win32Window::Win32Window()
		: DesktopWindow()
		, _updateJob(this, &Win32Window::Update, JobQueue::Queue::FramedNormalPriority, false, Thread::GetCurrentThreadId())
	{
		_hWndThreadId = Thread::GetCurrentThreadId();

		_hInstance = ::GetModuleHandle(NULL);

		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = _hInstance;
		windowClass.hIcon = NULL;
		windowClass.hCursor = ::LoadCursor(_hInstance, IDC_ARROW);
		windowClass.hbrBackground = ::CreateSolidBrush(RGB(0, 0, 0));
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = className;
		windowClass.hIconSm = NULL;

		_class = ::RegisterClassEx(&windowClass);
		if (_class == 0)
		{
			OUTPUT_ERROR("DesktopWindow: Unable to RegisterClass -> %s", GetLastErrorMessage().c_str());
			return;
		}

		_hWnd = ::CreateWindowEx(
			0,
			className,
			"Window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			_hInstance,
			NULL
		);

		if (_hWnd == NULL)
		{
			OUTPUT_ERROR("DesktopWindow: Unable to CreateWindow -> %s", GetLastErrorMessage().c_str());
			return;
		}

		SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		::ShowWindow(_hWnd, SW_NORMAL);

		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::PreLogic);
	}

	/// @brief 
	Win32Window::~Win32Window()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::PreLogic);

		if (_updateJob.Cancel() == true)
		{
			_updateJob.Wait();
		}

		if (_hWnd != NULL)
		{
			::DestroyWindow(_hWnd);
		}

		if (_class != 0)
		{
			::UnregisterClass(className, _hInstance);
		}
	}

	/// @brief 
	void Win32Window::Update()
	{
		if (_hWndThreadId != Thread::GetCurrentThreadId())
		{
			return;
		}

		for (const std::function<void()> function : _runOnWin32Thread)
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
				int x = GetSystemMetrics(SM_CXSCREEN) * 0.5f - _width * 0.5f;
				int y = GetSystemMetrics(SM_CYSCREEN) * 0.5f - _height * 0.5f;
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
}
