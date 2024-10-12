#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Resource.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace hod::renderer
{	
	class HOD_RENDERER_API TextureResource : public Resource
	{
		REFLECTED_CLASS(TextureResource, Resource, HOD_RENDERER_API)

	public:

							TextureResource() = default;
							TextureResource(const TextureResource&) = delete;
							TextureResource(TextureResource&&) = delete;
							~TextureResource() override;

		TextureResource&	operator = (const TextureResource&) = delete;
		TextureResource&	operator = (TextureResource&&) = delete;

	public:

		bool				Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		Texture*			GetTexture() const;

	public:

		uint16_t					_width = 0;
		uint16_t					_height = 0;
		uint8_t						_componentCount = 0;
		FilterMode					_filterMode = FilterMode::Nearest;
		WrapMode					_wrapMode = WrapMode::Clamp;

		Texture*					_texture = nullptr;
	};
}