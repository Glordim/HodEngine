#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventSystem/Event/Event.hpp"
#include "HodEngine/UI/NavigationDirection.hpp"

namespace hod::inline ui
{
	/// @brief 
	struct HOD_UI_API NavigationEvent : public Event
	{
		NavigationDirection _direction = NavigationDirection::Up;
	};
}
