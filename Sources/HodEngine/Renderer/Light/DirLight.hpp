#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod::renderer
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
