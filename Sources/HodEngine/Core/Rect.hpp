#pragma once

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct Rect
	{
		REFLECTED_CLASS_NO_VIRTUAL(Rect)

	public:
	
		Vector2	_position;
		Vector2	_size;
	};
}
