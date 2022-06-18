#pragma once

#include <cstdint>
#include <cstddef>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Texture
		{
		public:

			Texture();
			virtual ~Texture();

			bool LoadFromPath(const char* path);

			uint32_t	GetWidth() const;
			uint32_t	GetHeight() const;

			virtual bool BuildDepth(size_t width, size_t height) = 0;
			virtual bool BuildBuffer(size_t width, size_t height, unsigned char* buffer) = 0;

		protected:

			uint32_t	_width = 0;
			uint32_t	_height = 0;
		};
	}
}
