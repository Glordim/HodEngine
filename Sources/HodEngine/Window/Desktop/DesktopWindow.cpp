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
}
