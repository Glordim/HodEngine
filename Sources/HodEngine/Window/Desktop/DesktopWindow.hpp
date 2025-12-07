#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Window.hpp"

#include "HodEngine/Window/MouseButton.hpp"
#include "HodEngine/Window/ScanCode.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>

#include <array>

namespace hod::window
{
	class Cursor;
	class IDesktopWindowInputListener;

	/// @brief
	class HOD_WINDOW_API DesktopWindow : public Window
	{
	public:
		DesktopWindow();

		virtual void SetSize(uint16_t width, uint16_t height) = 0;
		virtual void CenterToScreen() = 0;
		virtual void Maximize() = 0;

		virtual void SetVisible(bool visible) = 0;

		bool IsFocused() const;

		const Vector2& GetMousePosition() const;

		void    SetCursor(Cursor* cursor);
		Cursor* GetCursor() const;

		void RegisterInputListener(IDesktopWindowInputListener* inputListener);
		void UnregisterInputListener(IDesktopWindowInputListener* inputListener);

		void Close();

	protected:
		void SetFocusedInternal(bool focused);
		void SetMousePosition(const Vector2& mousePosition);

		void EmitKeyPressed(ScanCode scanCode);
		void EmitKeyReleased(ScanCode scanCode);
		void EmitMouseButtonPressed(MouseButton button);
		void EmitMouseButtonReleased(MouseButton button);
		void EmitMouseMoved(int x, int y);
		void EmitMouseScroll(int scroll);

	private:
		Vector2 _mousePosition;

		bool _focused = true;

		Cursor* _cursor = nullptr;

		Vector<IDesktopWindowInputListener*> _inputListeners;
	};
}
