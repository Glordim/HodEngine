#pragma once

#include "HodEngine/Core/Resource.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace hod::renderer
{
	class FontResource : public Resource
	{
		REFLECTED_CLASS(FontResource, Resource)

	public:

							FontResource() = default;
							FontResource(const FontResource&) = delete;
							FontResource(FontResource&&) = delete;
							~FontResource() override = default;

		FontResource&		operator = (const FontResource&) = delete;
		FontResource&		operator = (FontResource&&) = delete;

	public:

		bool				Initialize(const Document::Node& documentNode, std::istream& stream) override;
		void				Destroy() override;

		Texture*			GetTexture() const;

	public:

		Texture*					_texture = nullptr;
	};
}
