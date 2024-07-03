#pragma once

#include "HodEngine/Input/Feedback.hpp"

namespace hod::input
{
	class HOD_API FeedbackVibration : public Feedback
	{
		META_TYPE(FeedbackVibration, Feedback);

	public:
		float	_left = 1.0f;
		float	_right = 1.0f;
	};
}
