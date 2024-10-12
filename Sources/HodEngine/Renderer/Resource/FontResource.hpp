#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Resource.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"
#include <vector>

namespace hod::renderer
{
	class HOD_RENDERER_API FontResource : public Resource
	{
		REFLECTED_CLASS(FontResource, Resource, HOD_RENDERER_API)

	public:

		struct Kerning
		{
			char32_t	_code;
			uint32_t	_offsetX;
			uint32_t	_offsetY;
			uint32_t	_sizeX;
			uint32_t	_sizeY;
			uint32_t	_baseline;
		};

	public:

							FontResource() = default;
							FontResource(const FontResource&) = delete;
							FontResource(FontResource&&) = delete;
							~FontResource() override;

		FontResource&		operator = (const FontResource&) = delete;
		FontResource&		operator = (FontResource&&) = delete;

	public:

		bool				Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		Texture*			GetTexture() const;
		const Kerning&		GetKerning(char32_t code) const;

	private:

		Texture*					_texture = nullptr;
		std::vector<Kerning>		_kernings;
		Kerning						_unknownKerning;
	};
}
