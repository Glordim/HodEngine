#pragma once

#include <Core/src/Color.h>

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
