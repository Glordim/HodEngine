#include <limits>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/WindowLib/WindowLibMouse.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

#undef max

namespace hod::inline input
{
	struct MouseState : public State
	{
		int16_t _delta[2];
		int8_t  _wheel;
		uint8_t _buttons;
	};

	WindowLibMouse::WindowLibMouse()
	: Mouse(UID::INVALID_UID, "Mouse", Product::UNKNOWN)
	{
	}

	bool WindowLibMouse::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void WindowLibMouse::OnButtonPressed(MouseButton button)
	{
		EditNextState<MouseState>()->_buttons |= (1 << std::to_underlying(button));
		MarkForCurrent();
	}

	void WindowLibMouse::OnButtonReleased(MouseButton button)
	{
		EditNextState<MouseState>()->_buttons &= ~(1 << std::to_underlying(button));
	}

	void WindowLibMouse::OnButtonMoved(int x, int y)
	{
		EditNextState<MouseState>()->_delta[0] += (_lastPosX - x);
		EditNextState<MouseState>()->_delta[1] += (_lastPosY - y);

		_lastPosX = x;
		_lastPosY = y;
	}

	void WindowLibMouse::OnButtonScroll(int scroll)
	{
		EditNextState<MouseState>()->_wheel += scroll;
	}

	void WindowLibMouse::ResetNextState()
	{
		EditNextState<MouseState>()->_delta[0] = 0;
		EditNextState<MouseState>()->_delta[1] = 0;
		EditNextState<MouseState>()->_wheel = 0;
	}
}
