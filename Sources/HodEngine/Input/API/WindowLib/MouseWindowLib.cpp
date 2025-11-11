#include <array>
#include <limits>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/WindowLib/MouseWindowLib.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

#undef max

namespace hod::input
{
	struct MouseState : public State
	{
		int16_t _delta[2];
		int8_t  _wheel;
		uint8_t _buttons;
	};

	MouseWindowLib::MouseWindowLib()
	: Mouse(UID::INVALID_UID, "Mouse", Product::UNKNOWN)
	{
	}

	bool MouseWindowLib::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void MouseWindowLib::OnButtonPressed(window::MouseButton button)
	{
		EditNextState<MouseState>()->_buttons |= (1 << std::to_underlying(button));
		MarkForCurrent();
	}

	void MouseWindowLib::OnButtonReleased(window::MouseButton button)
	{
		EditNextState<MouseState>()->_buttons &= ~(1 << std::to_underlying(button));
	}

	void MouseWindowLib::OnButtonMoved(int x, int y)
	{
		EditNextState<MouseState>()->_delta[0] = x;
		EditNextState<MouseState>()->_delta[1] = y;
	}

	void MouseWindowLib::OnButtonScroll(int scroll)
	{
		EditNextState<MouseState>()->_wheel = scroll;
	}
}
