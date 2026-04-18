#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod::inline renderer
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
