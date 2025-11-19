#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/Event.hpp"
#include <HodEngine/Core/Job/Thread.hpp>

struct HWND__;
using HWND = HWND__*;
struct HINSTANCE__;
using HINSTANCE = HINSTANCE__*;
using UINT = unsigned int;
using WPARAM = uint64_t;
using LPARAM = int64_t;
using LRESULT = int64_t;

namespace hod::window
{
	/// @brief
	class HOD_WINDOW_API Win32Window : public DesktopWindow
	{
	public:
		Event<HWND, UINT, WPARAM, LPARAM> OnWinProc;
		static LRESULT                    WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		Win32Window(bool hidden = false);
		~Win32Window() override;

		void Update() override;

		HWND      GetWindowHandle() const;
		HINSTANCE GetInstanceHandle() const;

		void SetSize(uint16_t width, uint16_t height) override;
		void CenterToScreen() override;
		void Maximize() override;

		void SetVisible(bool visible) override;

		void       RunOnWin32Thread(std::function<void()> codeToRun);
		Thread::Id GetMessageLoopThreadId() const;

	private:
		LRESULT InternalWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		HINSTANCE _hInstance = nullptr;
		HWND      _hWnd = nullptr;

		Thread::Id _hWndThreadId;

		Vector<std::function<void()>> _runOnWin32Thread;
	};
}
