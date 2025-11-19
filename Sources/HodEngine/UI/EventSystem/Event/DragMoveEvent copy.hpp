#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventSystem/Event/Event.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::ui
{
	/// @brief 
	struct HOD_UI_API DragMoveEvent : public Event
	{
		Vector2 _position;
	};
}
