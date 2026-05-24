#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventSystem/Event/Event.hpp"
#include "HodEngine/UI/EventSystem/Event/FocusEvent.hpp"

namespace hod::inline ui
{
	struct HOD_UI_API UnfocusEvent : public Event
	{
		enum class Reason
		{
			FocusAnother,
			ClickedInVoid,
			PointerExit,
			Programmatically,
			Disabled,
		};

		Reason				_reason;
		FocusEvent::Reason	_focusAnotherReason;
	};
}
