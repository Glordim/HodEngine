#pragma once

#include <HodEngine/Core/Src/Color.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct DirLight
		{
			DirLight()
			{

			}

			CORE::Color color;
			float intensity;
		};
	}
}
