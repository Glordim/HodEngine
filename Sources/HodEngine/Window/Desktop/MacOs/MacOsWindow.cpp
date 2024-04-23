#if defined(PLATFORM_MACOS)

#include "MacOsWindow.hpp"

#include <string>

#include <HodEngine/Core/Output.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <HodEngine/Renderer/RHI/Context.hpp>

#include <cstdlib>

using namespace hod::renderer;

namespace hod::window
{
	/// @brief 
	MacOsWindow::MacOsWindow()
		: DesktopWindow()
		, _updateJob(this, &MacOsWindow::Update, JobQueue::Queue::FramedNormalPriority, false, Thread::GetCurrentThreadId())
	{
		
		
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

#endif
