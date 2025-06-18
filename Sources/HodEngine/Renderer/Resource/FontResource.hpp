#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Resource/Resource.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Core/Vector.hpp"

struct FT_FaceRec_;

namespace hod::renderer
{
	class Font;

	class HOD_RENDERER_API FontResource : public Resource
	{
		REFLECTED_CLASS(FontResource, Resource)

	public:

							FontResource() = default;
							FontResource(const FontResource&) = delete;
							FontResource(FontResource&&) = delete;
							~FontResource() override;

		FontResource&		operator = (const FontResource&) = delete;
		FontResource&		operator = (FontResource&&) = delete;

	public:

		bool				Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		Font*				GetFont() const;

	private:

		Font*				_font = nullptr;
	};
}
