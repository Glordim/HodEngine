#pragma once

#include "HodEngine/Math/Vector2.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct BoundingBox
		{
			math::Vector2 center;
			math::Vector2 size;
			math::Vector2 min;
			math::Vector2 max;
		};
	}
}
