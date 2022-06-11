#pragma once

#include <HodEngine/Core/Src/Color.h>

namespace HOD
{
	namespace RENDERER
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
