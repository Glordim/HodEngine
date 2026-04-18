#pragma once

#include "HodEngine/Input/Feedback.hpp"

namespace hod::inline input
{
	class HOD_INPUT_API FeedbackVibration : public Feedback
	{
		RTTI(FeedbackVibration, Feedback)

	public:
		float _left = 1.0f;
		float _right = 1.0f;
	};
}
