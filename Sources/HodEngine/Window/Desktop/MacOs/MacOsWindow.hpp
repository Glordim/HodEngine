#pragma once

#if defined(PLATFORM_MACOS)

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/Job/MemberFunctionJob.hpp"

#include "HodEngine/Core/Event.hpp"

#include <string>

#ifdef __OBJC__
@class NSView;
@class NSWindow;
#else
class NSView;
class NSWindow;
#endif

namespace hod::window
{
	/// @brief 
	class MacOsWindow : public DesktopWindow
	{
	public:
											MacOsWindow();
											~MacOsWindow() override;

		void								Update() override;

		void								SetSize(uint16_t width, uint16_t height) override;
		void								CenterToScreen() override;
		void								Maximize() override;

		NSView* 							GetNsView() const;

	private:

		NSView*								_view = nullptr;
		NSWindow*							_window = nullptr;
	};
}

#endif
