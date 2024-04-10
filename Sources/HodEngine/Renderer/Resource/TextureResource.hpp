#pragma once

#include "HodEngine/Core/Resource.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace hod::renderer
{	
	class TextureResource : public Resource
	{
		REFLECTED_CLASS(TextureResource, Resource)

	public:

		enum Filtering
		{
			Nearest,
			Linear,
			
			Count,
		};


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

		uint16_t					_width = 0;
		uint16_t					_height = 0;
		uint8_t						_componentCount = 0;
		Filtering					_filtering = Filtering::Nearest;

		Texture*					_texture = nullptr;
	};
}