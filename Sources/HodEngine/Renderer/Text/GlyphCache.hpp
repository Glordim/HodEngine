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

	/// @brief The glyphs drawn so far, rasterized by FreeType into the pages of an R8 atlas, the first time they are asked for.
	/// Kept by the id of their FontFace: Forget it before the face is destroyed or loaded again.
	class HOD_RENDERER_API GlyphCache final
	{
	public:

		enum class Mode : uint8_t
		{
			Sdf,    // a signed distance field: any size, effects (outline, glow...). 128 is the outline, more is inside
			Bitmap, // coverage, rasterized at the size it is drawn at (small sizes, pixel art)
		};

		static constexpr uint32_t PageSize = 1024;
		static constexpr uint32_t SdfEmSize = 48; // the em, in pixels, the SDF glyphs are rasterized at
		static constexpr uint32_t SdfSpread = 6;  // the distance, in pixels at SdfEmSize, the SDF goes to on each side of the outline

		struct Glyph
		{
			uint32_t	_pageIndex = 0;
			Vector2		_uvMin; // in the page: (0, 0) its first texel, v down
			Vector2		_uvMax;
			Vector2		_boundsMin; // of its quad, from the origin of the glyph, in ems (x right, y up): the quad at a size is bounds * size
			Vector2		_boundsMax;
			bool		_empty = false; // nothing to draw (a space): no quad, nothing in the atlas
		};

	public:

						GlyphCache() = default;
						GlyphCache(const GlyphCache&) = delete;
						GlyphCache(GlyphCache&&) = delete;
						~GlyphCache();

		GlyphCache&		operator = (const GlyphCache&) = delete;
		GlyphCache&		operator = (GlyphCache&&) = delete;

	public:

		bool			Init();
		void			Clear(); // every glyph, page, and face

		// pixelSize: the em in pixels, Bitmap only. Null when it can't be rasterized.
		// Valid until the face is forgotten or the cache cleared.
		const Glyph*	GetGlyph(const FontFace& face, uint32_t glyphId, Mode mode, uint32_t pixelSize = 0);

		// The space of its glyphs in the atlas is not given back
		void			Forget(const FontFace& face);

		uint32_t		GetPageCount() const;
		Texture*		GetPage(uint32_t pageIndex) const;

		uint32_t		GetRasterizedGlyphCount() const; // since Init: how many times FreeType was asked to rasterize

	private:

		struct Page
		{
			Texture*		_texture = nullptr;
			SkylinePacker*	_packer = nullptr;
		};

	private:

		FT_FaceRec_*	GetFtFace(const FontFace& face);
		bool			Allocate(uint32_t width, uint32_t height, uint32_t& pageIndex, uint32_t& x, uint32_t& y);
		bool			AddPage();

	private:

		FT_LibraryRec_*								_ftLibrary = nullptr;
		std::unordered_map<uint32_t, FT_FaceRec_*>	_ftFaces; // by FontFace id

		std::unordered_map<uint64_t, Glyph>			_glyphs;
		Vector<Page>								_pages;

		Vector<uint8_t>								_pixels; // of the glyph being copied into a page
		uint32_t									_rasterizedGlyphCount = 0;
	};
}
