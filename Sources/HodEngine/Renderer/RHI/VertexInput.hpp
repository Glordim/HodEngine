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
				R32G32B32A32_SFloat,
				A8B8G8R8_UNorm_Pack32,
				Count
			};

			/// @brief 
			VertexInput()
			: _offset(0)
			, _format(R32G32_SFloat)
			{
				
			}

			/// @brief 
			/// @param offset 
			/// @param format 
			VertexInput(uint32_t offset, Format format)
			: _offset(offset)
			, _format(format)
			{

			}

		public:
			
			uint32_t	_offset;
			Format		_format;
		};
	}
}
