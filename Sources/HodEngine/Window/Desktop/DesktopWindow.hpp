#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Window.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Event.hpp"

#include <array>

namespace hod::window
{
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

		virtual void			SetSize(uint16_t width, uint16_t height) = 0;
		virtual void			CenterToScreen() = 0;
		virtual void			Maximize() = 0;

		virtual void			SetVisible(bool visible) = 0;

		bool					IsFocused() const;
		Event<bool>&			GetFocusedEvent();

		const Vector2&			GetMousePosition() const;
		bool					GetMouseButton(MouseButton button) const;

		void					SetMousePosition(const Vector2& mousePosition);
		void					SetMouseButton(MouseButton button, bool pressed);

		void					Close();

	protected:

		void					SetFocused(bool focused);

    private:

    	Vector2 								_mousePosition;
        std::array<bool, MouseButton::Count>	_mouseButtons;

		bool					_close = false;
		bool					_focused = true;

		Event<bool>				_focusEvent;
	};
}
