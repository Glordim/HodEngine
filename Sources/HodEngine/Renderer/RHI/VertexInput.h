#pragma once

#include <map>
#include <string>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

namespace hod
{
	namespace renderer
	{
		/// @brief 
		class VertexInput
		{
		public:

			enum Format
			{
				R32G32_SFloat = 0,
				A8B8G8R8_UNorm_Pack32,
				Count
			};

		public:
			
			uint32_t	_offset;
			Format		_format;
		};
	}
}
