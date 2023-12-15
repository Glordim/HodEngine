#pragma once

#include <HodEngine/Core/Color.hpp>

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

			Color color;
			float intensity;
			float range;
		};
	}
}
