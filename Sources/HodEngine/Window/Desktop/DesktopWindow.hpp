#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Window.hpp"

#include "HodEngine/Window/MouseButton.hpp"
#include "HodEngine/Window/ScanCode.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>

#include <array>

namespace hod::window
{
	class IDesktopWindowInputListener;

	/// @brief
	class HOD_WINDOW_API DesktopWindow : public Window
	{
	public:
		virtual void SetSize(uint16_t width, uint16_t height) = 0;
		virtual void CenterToScreen() = 0;
		virtual void Maximize() = 0;

		virtual void SetVisible(bool visible) = 0;

		bool         IsFocused() const;
		Event<bool>& GetFocusedEvent();

		const Vector2& GetMousePosition() const;

		void SetMousePosition(const Vector2& mousePosition);

		void RegisterInputListener(IDesktopWindowInputListener* inputListener);
		void UnregisterInputListener(IDesktopWindowInputListener* inputListener);

		void Close();

	protected:
		void SetFocused(bool focused);

	public:
		void EmitKeyPressed(ScanCode scanCode);
		void EmitKeyReleased(ScanCode scanCode);
		void EmitMouseButtonPressed(MouseButton button);
		void EmitMouseButtonReleased(MouseButton button);
		void EmitMouseMoved(int x, int y);
		void EmitMouseScroll(int scroll);

	private:
		Vector2 _mousePosition;

		bool        _focused = true;
		Event<bool> _focusEvent;

		Vector<IDesktopWindowInputListener*> _inputListeners;
	};
}
