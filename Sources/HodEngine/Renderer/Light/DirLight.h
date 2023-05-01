#pragma once

#include <HodEngine/Core/Color.h>

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
