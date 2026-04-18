#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod::renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct PointLight
	{
		PointLight()
		{

		}

		Color color;
		float intensity;
		float range;
	};
}
