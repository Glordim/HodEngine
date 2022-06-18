#pragma once

#include <HodEngine/Core/Src/Color.h>

namespace hod
{
	namespace renderer
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
