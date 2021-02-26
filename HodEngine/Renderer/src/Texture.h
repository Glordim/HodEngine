#pragma once

namespace HOD
{
	namespace RENDERER
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

			virtual bool BuildDepth(size_t width, size_t height) = 0;
			virtual bool BuildBuffer(size_t width, size_t height, unsigned char* buffer) = 0;
		};
	}
}
