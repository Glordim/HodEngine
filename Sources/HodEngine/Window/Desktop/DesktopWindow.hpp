#pragma once

#include "HodEngine/Window/Window.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

#include <array>

namespace hod::window
{
	/// @brief 
	class DesktopWindow : public Window
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

		const Vector2&			GetMousePosition() const;
		bool					GetMouseButton(MouseButton button) const;

		void					SetMousePosition(const Vector2& mousePosition);
		void					SetMouseButton(MouseButton button, bool pressed);

		void					Close();

    private:

    	Vector2 								_mousePosition;
        std::array<bool, MouseButton::Count>	_mouseButtons;

		bool					_close = false;
	};
}
