#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/GameSystems/Job/MemberFunctionJob.hpp"

#include "HodEngine/Core/Event.hpp"

#include "HodEngine/Core/String.hpp"

#include <functional>

#ifdef __OBJC__
@class NSView;
@class NSWindow;
@class MyWindowDelegate;
#else
class NSView;
class NSWindow;
class MyWindowDelegate;
#endif

namespace hod::inline window
{
	class MacOsWindow;

    class MacOsWindowEventCaller
    {
    public:
        static void EmitKeyPressed(MacOsWindow* window, uint16_t keyCode);
        static void EmitKeyReleased(MacOsWindow* window, uint16_t keyCode);
        static void EmitChar(MacOsWindow* window, char c);

        static void EmitMouseButtonPressed(MacOsWindow* window, MouseButton button);
        static void EmitMouseButtonReleased(MacOsWindow* window, MouseButton button);

        static void EmitMouseMoved(MacOsWindow* window, float x, float y);
        static void EmitMouseScroll(MacOsWindow* window, float scroll);
        static void EmitMouseHorizontalScroll(MacOsWindow* window, float scroll);

		static void EmitResize(MacOsWindow* window, float x, float y);
    };

	/// @brief 
	class HOD_WINDOW_API MacOsWindow : public DesktopWindow
	{
		friend class MacOsWindowEventCaller;
		
	public:
											MacOsWindow(bool hidden);
											~MacOsWindow() override;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

		void								SetVisible(bool visible) override;

		void								SetTitle(const char* title) override;
		void								SetDecoration(bool decoration) override;

		void								SetPosition(const Vector2& position) override;

		NSView* 							GetNsView() const;

		float								GetScaleFactor() const override;

		void								ResizeContext();

		void								UpdatePositionFromNative();

	private:

		// Executes codeToRun on the main thread: inline if already on it,
		// dispatched otherwise (AppKit calls must happen on the main thread).
		void								RunOnMainThread(std::function<void()> codeToRun);

		NSView*								_view = nullptr;
		NSWindow*							_window = nullptr;
		MyWindowDelegate*					_delegate = nullptr;
	};
}
