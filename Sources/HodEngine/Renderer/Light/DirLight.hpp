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

		Color color;
		float intensity;
	};
}
