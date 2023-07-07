#pragma once

#include <HodEngine/Core/Color.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct SpotLight
		{
			SpotLight()
			{

			}

			Color color;
			float intensity;
			float radius;
			float outer;
			float inner;
		};
	}
}
