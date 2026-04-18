#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventSystem/Event/Event.hpp"
#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline ui
{
	/// @brief 
	struct HOD_UI_API DragBeginEvent : public Event
	{
		Vector2 _position;
	};
}
