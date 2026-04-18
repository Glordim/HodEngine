#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod::renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct SpotLight
	{
		SpotLight()
		{

		}

		math::Color color;
		float intensity;
		float radius;
		float outer;
		float inner;
	};
}
