#pragma once

#include "HodEngine/Core/Resource.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace hod::renderer
{	
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

		bool				Initialize(const Document::Node& documentNode, std::istream& stream) override;
		void				Destroy() override;

		Texture*			GetTexture() const;

	public:

		uint16_t					_width;
		uint16_t					_height;
		uint8_t						_componentCount;

		Texture*					_texture = nullptr;
	};
}