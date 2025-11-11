#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Window.hpp"

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
		enum MouseButton
		{
			Left = 0,
			Middle,
			Right,

			Count
		};

	public:
		virtual void SetSize(uint16_t width, uint16_t height) = 0;
		virtual void CenterToScreen() = 0;
		virtual void Maximize() = 0;

		virtual void SetVisible(bool visible) = 0;

		bool         IsFocused() const;
		Event<bool>& GetFocusedEvent();

		const Vector2& GetMousePosition() const;
		bool           GetMouseButton(MouseButton button) const;

		void SetMousePosition(const Vector2& mousePosition);
		void SetMouseButton(MouseButton button, bool pressed);

		void RegisterInputListener(IDesktopWindowInputListener* inputListener);
		void UnregisterInputListener(IDesktopWindowInputListener* inputListener);

		void Close();

	protected:
		void SetFocused(bool focused);

	public:
		void EmitKeyPressed(ScanCode scanCode);
		void EmitKeyReleased(ScanCode scanCode);
		void EmitMouseButtonPressed(int button);
		void EmitMouseButtonReleased(int button);
		void EmitMouseMoved(int x, int y);

	private:
		Vector2 _mousePosition;

		bool        _focused = true;
		Event<bool> _focusEvent;

		Vector<IDesktopWindowInputListener*> _inputListeners;
	};
}
