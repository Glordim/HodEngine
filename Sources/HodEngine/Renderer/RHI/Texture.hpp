#pragma once

#include <cstdint>
#include <cstddef>

#include <HodEngine/Core/Color.hpp>

namespace hod
{
	class Vector2;

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

			virtual bool BuildColor(size_t width, size_t height, bool allowReadWrite = false) = 0;
			virtual bool BuildDepth(size_t width, size_t height) = 0;
			virtual bool BuildBuffer(size_t width, size_t height, unsigned char* buffer) = 0;

			virtual Color ReadPixel(const Vector2& position) const = 0;

		protected:

			uint32_t	_width = 0;
			uint32_t	_height = 0;
		};
	}
}
