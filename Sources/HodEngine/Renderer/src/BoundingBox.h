#pragma once

#include <glm/vec3.hpp>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct BoundingBox
		{
			glm::vec3 center;
			glm::vec3 size;
			glm::vec3 min;
			glm::vec3 max;
		};
	}
}
