#pragma once

#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct BoundingBox
	{
		Vector2 center;
		Vector2 size;
		Vector2 min;
		Vector2 max;
	};
}
