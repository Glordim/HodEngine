#include "HodEngine/Window/Pch.hpp"
#include "DesktopDisplayManager.hpp"
#include "DesktopWindow.hpp"

namespace hod::window
{
	DesktopWindow::DesktopWindow()
	{
		_cursor = DesktopDisplayManager::GetInstance()->GetBultinCursor(DesktopDisplayManager::BuiltinCursor::Arrow);
	}

	void DesktopWindow::SetCursor(Cursor* cursor)
	{
		if (cursor == nullptr)
		{
			_cursor = DesktopDisplayManager::GetInstance()->GetBultinCursor(DesktopDisplayManager::BuiltinCursor::Arrow);
		}
		else
		{
			_cursor = cursor;
		}
	}

	Cursor* DesktopWindow::GetCursor() const
	{
		return _cursor;
	}

	/// @brief
	/// @param mousePosition
	void DesktopWindow::SetMousePosition(const Vector2& mousePosition)
	{
		_mousePosition = mousePosition;
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
	bool DesktopWindow::IsFocused() const
	{
		return _focused;
	}

	/// @brief
	/// @param focused
	void DesktopWindow::SetFocusedInternal(bool focused)
	{
		if (_focused != focused)
		{
			_focused = focused;

			Event event;
			event.type = focused ? EventType::FocusGained : EventType::FocusLost;
			EnqueueEvent(event);
		}
	}

	void DesktopWindow::EmitKeyPressed(ScanCode scanCode, Key key)
	{
		Event event;
		event.type = EventType::KeyPressed;
		event.data.key.scanCode = scanCode;
		event.data.key.key = key;
		event.data.key.flags = 0; // todo
		EnqueueEvent(event);
	}

	void DesktopWindow::EmitKeyReleased(ScanCode scanCode, Key key)
	{
		Event event;
		event.type = EventType::KeyReleased;
		event.data.key.scanCode = scanCode;
		event.data.key.key = key;
		event.data.key.flags = 0; // todo
		EnqueueEvent(event);
	}

	void DesktopWindow::EmitMouseButtonPressed(MouseButton button)
	{
		Event event;
		event.type = EventType::MouseButtonDown;
		event.data.mouseButton.button = button;
		event.data.mouseButton.x = 0; // todo
		event.data.mouseButton.y = 0; // todo
		EnqueueEvent(event);
	}

	void DesktopWindow::EmitMouseButtonReleased(MouseButton button)
	{
		Event event;
		event.type = EventType::MouseButtonUp;
		event.data.mouseButton.button = button;
		event.data.mouseButton.x = 0; // todo
		event.data.mouseButton.y = 0; // todo
		EnqueueEvent(event);
	}

	void DesktopWindow::EmitMouseMoved(int x, int y)
	{
		Event event;
		event.type = EventType::MouseMoved;
		event.data.mouseMove.x = x;
		event.data.mouseMove.y = y;
		EnqueueEvent(event);
	}

	void DesktopWindow::EmitMouseScroll(float scroll)
	{
		Event event;
		event.type = EventType::MouseScroll;
		event.data.mouseScroll.value = scroll;
		EnqueueEvent(event);
	}

	void DesktopWindow::EmitMouseHorizontalScroll(float scroll)
	{
		Event event;
		event.type = EventType::MouseHorizontalScroll;
		event.data.mouseScroll.value = scroll;
		EnqueueEvent(event);
	}
}
