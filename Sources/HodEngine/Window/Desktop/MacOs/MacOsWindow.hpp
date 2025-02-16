#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/Job/MemberFunctionJob.hpp"

#include "HodEngine/Core/Event.hpp"

#include <string>

#ifdef __OBJC__
@class NSView;
@class NSWindow;
@class MyWindowDelegate;
#else
class NSView;
class NSWindow;
class MyWindowDelegate;
#endif

namespace hod::window
{
	/// @brief 
	class HOD_WINDOW_API MacOsWindow : public DesktopWindow
	{
	public:
											MacOsWindow(bool hidden);
											~MacOsWindow() override;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

		void								SetVisible(bool visible) override;

		NSView* 							GetNsView() const;

		float								GetScaleFactor() const;

		void								ResizeContext();

	private:

		NSView*								_view = nullptr;
		NSWindow*							_window = nullptr;
		MyWindowDelegate*					_delegate = nullptr;
	};
}
