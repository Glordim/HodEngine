#pragma once

#include <HodEngine/Math/Color.hpp>

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

			math::Color color;
			float intensity;
		};
	}
}
