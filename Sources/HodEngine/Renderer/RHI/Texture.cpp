#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <Stb/stb_image.h>

namespace hod::renderer
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

		uint8_t* buffer = stbi_load(path, &width, &height, &channel, 4);
		if (buffer == nullptr)
		{
			OUTPUT_ERROR("Texture : Failed to load Texture \"{}\"", path);
			return false; // Todo Memleak
		}

		CreateInfo createInfo;
		if (BuildBuffer(width, height, buffer, createInfo) == false)
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
