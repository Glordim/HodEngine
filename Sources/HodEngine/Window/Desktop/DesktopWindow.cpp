#include "DesktopWindow.hpp"

namespace hod::window
{
    /// @brief 
	/// @param mousePosition 
	void DesktopWindow::SetMousePosition(const Vector2& mousePosition)
	{
		_mousePosition = mousePosition;
	}

    /// @brief 
    /// @param button 
    /// @param pressed 
    void DesktopWindow::SetMouseButton(MouseButton button, bool pressed)
    {
        _mouseButtons[button] = pressed;
    }

	/// @brief 
	/// @return 
	const Vector2& DesktopWindow::GetMousePosition() const
	{
		return _mousePosition;
	}

    /// @brief 
    /// @param button 
    /// @return 
    bool DesktopWindow::GetMouseButton(MouseButton button) const
    {
        return _mouseButtons[button];
    }

	/// @brief 
	void DesktopWindow::Close()
    {
        _close = true;
    }

    /// @brief 
    /// @return 
    Event<bool>& DesktopWindow::GetFocusedEvent()
    {
        return _focusEvent;
    }

    /// @brief 
    /// @return 
    bool DesktopWindow::IsFocused() const
    {
        return _focused;
    }

    /// @brief 
    /// @param focused 
    void DesktopWindow::SetFocused(bool focused)
    {
        if (_focused != focused)
        {
            _focused = focused;
            _focusEvent.Emit(focused);
        }
    }
}
