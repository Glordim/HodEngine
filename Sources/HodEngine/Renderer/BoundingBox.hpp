#pragma once

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod
{
	namespace renderer
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
}
