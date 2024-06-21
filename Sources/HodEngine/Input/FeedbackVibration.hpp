#pragma once

#include "HodEngine/Input/Feedback.hpp"

namespace hod::input
{
	class FeedbackVibration : public Feedback
	{
		META_TYPE(FeedbackVibration, Feedback);

	public:
		float	_left = 1.0f;
		float	_right = 1.0f;
	};
}
