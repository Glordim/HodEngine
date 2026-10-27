#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include "HodEngine/Renderer/Text/GlyphCache.hpp"

#include <HodEngine/Text/FontFace.hpp>

namespace hod::inline renderer
{
	/// @brief A font file, read once into a FontFace, and how its glyphs are drawn.
	/// Its glyphs drawn so far are in its own GlyphCache (its atlas), which goes with it.
	class HOD_RENDERER_API FontResource : public Resource
	{
		REFLECTED_CLASS(FontResource, Resource)

	public:

		enum class RenderMode : uint8_t
		{
			Smooth,   // anti-aliased, rasterized at the size it is drawn at
			PixelArt, // its glyphs on a grid of PixelSize pixels an em: drawn at whole multiples of it, sharp
		};
		REFLECTED_ENUM(HOD_RENDERER_API, RenderMode);

		/// @brief The weights of CSS (and of the OS/2 table of a font file)
		enum class Weight : uint16_t
		{
			Thin = 100,
			ExtraLight = 200,
			Light = 300,
			Regular = 400,
			Medium = 500,
			SemiBold = 600,
			Bold = 700,
			ExtraBold = 800,
			Black = 900,
		};
		REFLECTED_ENUM(HOD_RENDERER_API, Weight);

		/// @brief The font of the family drawn for a text asking for this style: another one, or the font itself
		struct Style
		{
			UID		_font;
			Weight	_weight = Weight::Regular;
			bool	_italic = false;
		};

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
		GlyphCache&			GetGlyphCache(); // of its face

		// Changed: its atlas is emptied, its glyphs are drawn otherwise
		RenderMode			GetRenderMode() const;
		void				SetRenderMode(RenderMode renderMode);

		// PixelArt only: the size the font is drawn for, in pixels an em (Silkscreen: 8). 0 when not known. Changed in PixelArt: its atlas is emptied
		uint32_t			GetPixelSize() const;
		void				SetPixelSize(uint32_t pixelSize);

		// The size a text is drawn at for a size asked for: PixelArt, the closest whole multiple of PixelSize, at least once
		float				GetDrawnFontSize(float fontSize) const;

		// How its glyphs are rasterized: Pixel when PixelArt with a PixelSize, Bitmap otherwise
		GlyphCache::Mode	GetGlyphMode() const;

		// The fonts the characters it lacks are drawn with, in the order they are looked into (their own fallbacks after each of them). See FontChain
		const Vector<UID>&	GetFallbacks() const;
		void				SetFallbacks(const Vector<UID>& fallbacks);

		// Its family: the font drawn for each style, the same one for several maybe, the font itself among them maybe. None: the font itself for all.
		// Read on each font of a chain, not on a font one of them is drawn with for a style: see FontChain
		const Vector<Style>&	GetStyles() const;
		void				SetStyles(const Vector<Style>& styles);

		// The font of its styles closest to a style, as CSS matches it: the italic first, then the weight. An invalid UID without styles
		UID					FindStyle(Weight weight, bool italic) const;

	private:

		FontFace			_fontFace;
		GlyphCache			_glyphCache; // after the face: it reads it, it goes first
		RenderMode			_renderMode = RenderMode::Smooth;
		uint32_t			_pixelSize = 0;
		Vector<UID>			_fallbacks; // not locked here: fonts falling back on each other would keep each other alive
		Vector<Style>		_styles; // not locked either: only the one drawn is loaded
	};
}
