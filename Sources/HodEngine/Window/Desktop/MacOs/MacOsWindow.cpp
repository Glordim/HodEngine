#include "MacOsWindow.hpp"

#include "HodEngine/Core/String.hpp"

#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <cstdlib>

namespace hod::window
{
	/// @brief 
	/// @return 
	NSView* MacOsWindow::GetNsView() const
	{
		return _view;
	}

	void MacOsWindowEventCaller::EmitKeyPressed(MacOsWindow* window, ScanCode scanCode)
    {
        window->EmitKeyPressed(scanCode, Key::None); // toto
    }

    void MacOsWindowEventCaller::EmitKeyReleased(MacOsWindow* window, ScanCode scanCode)
    {
        window->EmitKeyReleased(scanCode, Key::None); // todo
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

    void MacOsWindowEventCaller::EmitResize(MacOsWindow* window, float x, float y)
    {
        window->ResizeInternal(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
    }
}
