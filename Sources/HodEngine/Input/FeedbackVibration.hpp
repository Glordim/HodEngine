#pragma once

#include "HodEngine/Input/Feedback.hpp"

namespace hod::inline input
{
	/// @brief The rumble of a gamepad, each motor's speed in [0, 1] (0: stopped)
	class HOD_INPUT_API FeedbackVibration : public Feedback
	{
		RTTI(FeedbackVibration, Feedback)

	public:
		float _left = 1.0f;  // The low frequency motor, in the left grip
		float _right = 1.0f; // The high frequency motor, in the right grip

		// The motors in the triggers (Xbox One and Series gamepads, through GameInput only: ignored without them or with XInput)
		float _leftTrigger = 0.0f;
		float _rightTrigger = 0.0f;
	};
}
