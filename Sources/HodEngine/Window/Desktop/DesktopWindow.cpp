#include "HodEngine/Window/Pch.hpp"
#include "DesktopWindow.hpp"

#include "IDesktopWindowInputListener.hpp"

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
		if (pressed)
		{
			EmitMouseButtonPressed(button);
		}
		else
		{
			EmitMouseButtonReleased(button);
		}
	}

	/// @brief
	/// @return
	const Vector2& DesktopWindow::GetMousePosition() const
	{
		return _mousePosition;
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

	void DesktopWindow::RegisterInputListener(IDesktopWindowInputListener* inputListener)
	{
		_inputListeners.push_back(inputListener);
	}

	void DesktopWindow::UnregisterInputListener(IDesktopWindowInputListener* inputListener)
	{
		if (std::remove(_inputListeners.begin(), _inputListeners.end(), inputListener) == _inputListeners.end())
		{
			OUTPUT_ERROR("UnregisterInputListener: Not found"); // TODO
		}
	}

	void DesktopWindow::EmitKeyPressed(ScanCode scanCode)
	{
		for (IDesktopWindowInputListener* inputListener : _inputListeners)
		{
			inputListener->OnKeyPressed(scanCode);
		}
	}

	void DesktopWindow::EmitKeyReleased(ScanCode scanCode)
	{
		for (IDesktopWindowInputListener* inputListener : _inputListeners)
		{
			inputListener->OnKeyReleased(scanCode);
		}
	}

	void DesktopWindow::EmitMouseButtonPressed(int button)
	{
		for (IDesktopWindowInputListener* inputListener : _inputListeners)
		{
			inputListener->OnMouseButtonPressed(button);
		}
	}

	void DesktopWindow::EmitMouseButtonReleased(int button)
	{
		for (IDesktopWindowInputListener* inputListener : _inputListeners)
		{
			inputListener->OnMouseButtonReleased(button);
		}
	}

	void DesktopWindow::EmitMouseMoved(int x, int y)
	{
		for (IDesktopWindowInputListener* inputListener : _inputListeners)
		{
			inputListener->OnMouseMoved((float)x, (float)y);
		}
	}
}
