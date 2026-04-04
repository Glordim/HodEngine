#pragma once

#include <HodEngine/Math/Color.hpp>

namespace hod
{
	namespace renderer
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
}
