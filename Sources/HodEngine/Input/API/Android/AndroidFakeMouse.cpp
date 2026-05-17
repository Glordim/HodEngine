#include <array>
#include <limits>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/Android/AndroidFakeMouse.hpp"
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

	AndroidFakeMouse::AndroidFakeMouse()
	: Mouse(UID::INVALID_UID, "Mouse", Product::UNKNOWN)
	{
	}

	bool AndroidFakeMouse::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void AndroidFakeMouse::OnButtonPressed(MouseButton button)
	{
		EditNextState<MouseState>()->_buttons |= (1 << std::to_underlying(button));
		MarkForCurrent();
	}

	void AndroidFakeMouse::OnButtonReleased(MouseButton button)
	{
		EditNextState<MouseState>()->_buttons &= ~(1 << std::to_underlying(button));
	}

	void AndroidFakeMouse::ResetNextState()
	{
		EditNextState<MouseState>()->_delta[0] = 0;
		EditNextState<MouseState>()->_delta[1] = 0;
		EditNextState<MouseState>()->_wheel = 0;
	}
}
