#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_CORE_API Rect
	{
		REFLECTED_CLASS_NO_VIRTUAL(Rect, HOD_CORE_API)

	public:
	
		Vector2	_position;
		Vector2	_size;

		bool operator==(const Rect& right) const = default;
	};
}
