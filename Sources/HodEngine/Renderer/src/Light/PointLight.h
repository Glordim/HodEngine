#pragma once

#include <HodEngine/Core/Src/Color.h>

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

			CORE::Color color;
			float intensity;
			float range;
		};
	}
}
