#pragma once

#if defined(PLATFORM_MACOS)

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/Job/MemberFunctionJob.hpp"

#include "HodEngine/Core/Event.hpp"

#include <string>
#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.swift.hpp"

#ifdef __OBJC__
@class NSView;
#else
class NSView;
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

		hod_swift::MacOsNativeWindow			_nativeWinwdow;
	};
}

#endif
