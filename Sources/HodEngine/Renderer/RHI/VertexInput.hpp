#pragma once

#include <map>
#include <string>

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
