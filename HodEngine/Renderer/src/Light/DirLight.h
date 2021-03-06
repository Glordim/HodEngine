#pragma once

#include <Core/src/Color.h>

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
