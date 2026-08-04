#include "MacOsWindow.hpp"

#include "HodEngine/Core/String.hpp"

#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>

#include <cstdlib>

namespace hod::inline window
{
	/// @brief 
	/// @return 
	NSView* MacOsWindow::GetNsView() const
	{
		return _view;
	}

	void MacOsWindowEventCaller::EmitKeyPressed(MacOsWindow* window, uint16_t keyCode)
    {
        window->EmitKeyPressed(MacOSKeyCodeToScanCode(keyCode), MacOSKeyCodeToKey(keyCode));
    }

    void MacOsWindowEventCaller::EmitKeyReleased(MacOsWindow* window, uint16_t keyCode)
    {
        window->EmitKeyReleased(MacOSKeyCodeToScanCode(keyCode), MacOSKeyCodeToKey(keyCode));
    }

    void MacOsWindowEventCaller::EmitChar(MacOsWindow* window, char c)
    {
        window->EmitChar(c);
    }

    void MacOsWindowEventCaller::EmitMouseButtonPressed(MacOsWindow* window, MouseButton button)
    {
        window->EmitMouseButtonPressed(button);
    }

    void MacOsWindowEventCaller::EmitMouseButtonReleased(MacOsWindow* window, MouseButton button)
    {
        window->EmitMouseButtonReleased(button);
    }

    void MacOsWindowEventCaller::EmitMouseMoved(MacOsWindow* window, float x, float y)
    {
        window->EmitMouseMoved(x, y);
    }

    void MacOsWindowEventCaller::EmitMouseScroll(MacOsWindow* window, float scroll)
    {
        window->EmitMouseScroll(scroll);
    }

    void MacOsWindowEventCaller::EmitMouseHorizontalScroll(MacOsWindow* window, float scroll)
    {
        window->EmitMouseHorizontalScroll(scroll);
    }

    void MacOsWindowEventCaller::EmitResize(MacOsWindow* window, float x, float y)
    {
        window->ResizeInternal(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
    }
}
