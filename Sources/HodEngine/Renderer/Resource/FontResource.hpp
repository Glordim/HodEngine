#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/Text/FontFace.hpp>

namespace hod::inline renderer
{
	/// @brief A font file, read once into a FontFace. Its glyphs rasterized in the GlyphCache of the Renderer are forgotten with it.
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

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		const FontFace&		GetFontFace() const;

	private:

		void				ForgetGlyphs();

	private:

		FontFace			_fontFace;
	};
}
