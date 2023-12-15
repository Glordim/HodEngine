#pragma once

#include <HodEngine/Core/Color.hpp>

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

			Color color;
			float intensity;
		};
	}
}
