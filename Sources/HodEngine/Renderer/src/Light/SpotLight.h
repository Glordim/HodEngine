#pragma once

#include <HodEngine/Core/Src/Color.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct SpotLight
		{
			SpotLight()
			{

			}

			CORE::Color color;
			float intensity;
			float radius;
			float outer;
			float inner;
		};
	}
}
