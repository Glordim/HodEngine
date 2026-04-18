#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct SpotLight
	{
		SpotLight()
		{

		}

		Color color;
		float intensity;
		float radius;
		float outer;
		float inner;
	};
}
