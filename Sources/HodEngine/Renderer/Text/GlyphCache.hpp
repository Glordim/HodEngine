#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>
#include <unordered_map>

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace hod::inline rhi
{
	class Texture;
}

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline renderer
{
	class SkylinePacker;

	/// @brief The glyphs of a font drawn so far, rasterized by FreeType into its own R8 atlas, the first time they are asked for.
	/// One per font (a FontFaceResource has its own): it lives, and its memory goes, with the font.
	/// The atlas is one page as long as it can: it starts small and doubles, its glyphs copied, up to MaxPageSize, a text of the font drawn in one batch.
	/// Only then another page is added (thousands of glyphs: CJK).
	class HOD_RENDERER_API GlyphCache final
	{
	public:

		enum class Mode : uint8_t
		{
			Sdf,    // a signed distance field: any size, effects (outline, glow...). 128 is the outline, more is inside
			Bitmap, // coverage, rasterized at the size it is drawn at (small sizes)
			Pixel,  // pixel art: on or off, rasterized at the size the font is drawn for (its pixel grid), drawn at whole multiples of it with a Nearest filter
		};

		static constexpr uint32_t InitialPageSize = 256;
		static constexpr uint32_t MaxPageSize = 2048;
		static constexpr uint32_t SdfEmSize = 48; // the em, in pixels, the SDF glyphs are rasterized at
		static constexpr uint32_t SdfSpread = 6;  // the distance, in pixels at SdfEmSize, the SDF goes to on each side of the outline
		// Empty texels around each glyph, in its quad: a linear filter fades its edges out instead of the quad cutting them
		static constexpr uint32_t GlyphBorder = 1;

		struct Glyph
		{
			uint32_t	_pageIndex = 0;
			Vector2		_uvMin; // in the page: (0, 0) its first texel, v down. Change when the page grows, see GetGeneration
			Vector2		_uvMax;
			Vector2		_boundsMin; // of its quad (its border included), from the origin of the glyph, in ems (x right, y up): the quad at a size is bounds * size
			Vector2		_boundsMax;
			Vector2		_inkMin; // of the outline (what is drawn, without the SDF spread), from the origin of the glyph, in ems
			Vector2		_inkMax;
			bool		_empty = false; // nothing to draw (a space): no quad, nothing in the atlas, no ink
		};

	public:

						GlyphCache() = default;
						GlyphCache(const GlyphCache&) = delete;
						GlyphCache(GlyphCache&&) = delete;
						~GlyphCache();

		GlyphCache&		operator = (const GlyphCache&) = delete;
		GlyphCache&		operator = (GlyphCache&&) = delete;

	public:

		// The face is read where it is: it outlives the cache, or the cache is cleared first
		bool			Init(const FontFace& face);
		void			Clear(); // every glyph and page, the face
		// Every glyph and page, the face kept: drawn otherwise from now on (another mode...). While a frame is recorded: its pages are kept until its end
		void			Reset();

		// pixelSize: the em in pixels, Bitmap and Pixel only (Pixel: the one of the grid of the font). Null when it can't be rasterized.
		// Valid until the cache is cleared
		const Glyph*	GetGlyph(uint32_t glyphId, Mode mode, uint32_t pixelSize = 0);

		uint32_t		GetPageCount() const;
		rhi::Texture*	GetPage(uint32_t pageIndex) const; // a page that grows is another texture
		uint32_t		GetPageSize(uint32_t pageIndex) const;

		// Unique to the state of the pages: changes when one grows (the uvs of its glyphs change, its texture too) or the cache is cleared.
		// What is made from the glyphs (meshes...) is made again then
		uint32_t		GetGeneration() const;

		uint32_t		GetRasterizedGlyphCount() const; // since Init: how many times FreeType was asked to rasterize

		// Every glyph rasterized so far, in every mode and size (an atlas viewer...). Valid until the cache is cleared
		void			GetGlyphs(Vector<const Glyph*>& glyphs) const;

	private:

		// Its pixels kept on the CPU too: copied into the larger texture when it grows
		struct Page
		{
			rhi::Texture*	_texture = nullptr;
			SkylinePacker*	_packer = nullptr;
			uint32_t		_size = 0;
			Vector<uint8_t>	_pixels;
		};

	private:

		bool			Allocate(uint32_t width, uint32_t height, uint32_t& pageIndex, uint32_t& x, uint32_t& y);
		bool			AddPage(uint32_t size);
		bool			GrowPage(uint32_t pageIndex);
		void			ReleasePages(bool untilTheEndOfTheFrame);
		rhi::Texture*	CreatePageTexture(uint32_t size, const uint8_t* pixels);

	private:

		FT_LibraryRec_*						_ftLibrary = nullptr;
		FT_FaceRec_*						_ftFace = nullptr;

		std::unordered_map<uint64_t, Glyph>	_glyphs;
		Vector<Page>						_pages;
		uint32_t							_generation = 0;

		Vector<uint8_t>						_pixels; // of the glyph being copied into a page
		uint32_t							_rasterizedGlyphCount = 0;
	};
}
