#pragma once

#include <glm/vec3.hpp>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct BoundingBox
		{
			glm::vec3 center;
			glm::vec3 min;
			glm::vec3 max;
		};
	}
}
