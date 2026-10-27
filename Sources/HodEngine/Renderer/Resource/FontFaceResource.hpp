#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"

#include "HodEngine/Renderer/Text/GlyphCache.hpp"

#include <HodEngine/Text/FontFace.hpp>

namespace hod::inline renderer
{
	/// @brief A font file, read once into a FontFace, and how its glyphs are drawn.
	/// Its glyphs drawn so far are in its own GlyphCache (its atlas), which goes with it.
	/// A text is not drawn with a face but with a FontResource, the family the face is one of the styles of.
	class HOD_RENDERER_API FontFaceResource : public Resource
	{
		REFLECTED_CLASS(FontFaceResource, Resource)

	public:

		enum class RenderMode : uint8_t
		{
			Smooth,   // anti-aliased, rasterized at the size it is drawn at
			PixelArt, // its glyphs on a grid of PixelSize pixels an em: drawn at whole multiples of it, sharp
		};
		REFLECTED_ENUM(HOD_RENDERER_API, RenderMode);

	public:

							FontFaceResource() = default;
							FontFaceResource(const FontFaceResource&) = delete;
							FontFaceResource(FontFaceResource&&) = delete;
							~FontFaceResource() override;

		FontFaceResource&	operator = (const FontFaceResource&) = delete;
		FontFaceResource&	operator = (FontFaceResource&&) = delete;

	public:

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		const FontFace&		GetFontFace() const;
		GlyphCache&			GetGlyphCache(); // of its face

		// Changed: its atlas is emptied, its glyphs are drawn otherwise
		RenderMode			GetRenderMode() const;
		void				SetRenderMode(RenderMode renderMode);

		// PixelArt only: the size the face is drawn for, in pixels an em (Silkscreen: 8). 0 when not known. Changed in PixelArt: its atlas is emptied
		uint32_t			GetPixelSize() const;
		void				SetPixelSize(uint32_t pixelSize);

		// The size a text is drawn at for a size asked for: PixelArt, the closest whole multiple of PixelSize, at least once
		float				GetDrawnFontSize(float fontSize) const;

		// How its glyphs are rasterized: Pixel when PixelArt with a PixelSize, Bitmap otherwise
		GlyphCache::Mode	GetGlyphMode() const;

	private:

		FontFace			_fontFace;
		GlyphCache			_glyphCache; // after the face: it reads it, it goes first
		RenderMode			_renderMode = RenderMode::Smooth;
		uint32_t			_pixelSize = 0;
	};
}
