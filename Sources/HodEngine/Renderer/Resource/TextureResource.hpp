#pragma once

#include "HodEngine/Core/Resource.hpp"

namespace hod::renderer
{
	// TODO nest in TextureResource when reflection support it :(
	struct TextureInfo
	{
		REFLECTED_CLASS_NO_VIRTUAL(TextureInfo)

	public:
	
		uint32_t	_offset;
		uint32_t	_size;
	};
	
	class TextureResource : public Resource
	{
		REFLECTED_CLASS(TextureResource, Resource)

	public:

							TextureResource() = default;
							TextureResource(const TextureResource&) = delete;
							TextureResource(TextureResource&&) = delete;
							~TextureResource() override = default;

		TextureResource&	operator = (const TextureResource&) = delete;
		TextureResource&	operator = (TextureResource&&) = delete;

	public:

		uint16_t					_width;
		uint16_t					_height;
		uint8_t						_componentCount;

		std::vector<TextureInfo>	_textureInfos;
	};
}