#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventSystem/Event/Event.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::ui
{
	/// @brief 
	struct HOD_UI_API DropEvent : public Event
	{
		enum class Reason : uint8_t
		{
			UserRelease,
			Cancel,
		};
		Vector2	_position;
		Reason	_reason = Reason::UserRelease;
	};
}
