#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <HodEngine/Core/Output.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <string>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Texture::Texture()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Texture::~Texture()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Texture::LoadFromPath(const char* path)
		{
			int width;
			int height;
			int channel;

			unsigned char* buffer = stbi_load(path, &width, &height, &channel, 4);
			if (buffer == nullptr)
			{
				OUTPUT_ERROR("Texture : Failed to load Texture \"%s\"", path);
				return false; // Todo Memleak
			}

			if (BuildBuffer(width, height, buffer) == false)
			{
				return false; // Todo Memleak
			}

			_width = width;
			_height = height;

			stbi_image_free(buffer);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t Texture::GetWidth() const
		{
			return _width;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t Texture::GetHeight() const
		{
			return _height;
		}
	}
}
