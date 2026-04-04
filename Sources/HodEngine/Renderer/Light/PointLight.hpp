#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct PointLight
		{
			PointLight()
			{

			}

			math::Color color;
			float intensity;
			float range;
		};
	}
}
