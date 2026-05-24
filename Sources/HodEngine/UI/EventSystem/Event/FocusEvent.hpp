#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventSystem/Event/Event.hpp"

namespace hod::inline ui
{
	struct HOD_UI_API FocusEvent : public Event
	{
		enum class Reason
		{
			Navigation,
			Clicked,
			PointerEnter,
			Programmatically,
		};

		Reason				_reason;
		FocusEvent::Reason	_focusAnotherReason;
	};
}
