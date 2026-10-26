#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/Text/GlyphCache.hpp"

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline text
{
	struct TextLayout;
}

namespace hod::inline renderer
{
	/// @brief The quads that draw a TextLayout, one batch per atlas page they read (one per font, but for thousands of glyphs):
	/// a mesh, and a material instance with its page, per batch.
	/// Positions in the space of the layout (x right, y up), from an origin on the baseline of the start of the line.
	struct HOD_RENDERER_API TextMesh
	{
		struct Batch
		{
			GlyphCache*			_glyphCache = nullptr; // the one of the font of its glyphs
			uint32_t			_glyphCacheGeneration = 0;
			uint32_t			_pageIndex = 0;
			Vector<Vector2>		_positions;
			Vector<Vector2>		_uvs;
			Vector<uint16_t>	_indices;
		};

		Vector<Batch>	_batches;

		// The bounds of what is drawn (the outlines of the glyphs), in the space of the positions. Empty when nothing is drawn
		Vector2			_inkMin;
		Vector2			_inkMax;
		bool			HasInk() const;

		// glyphCaches: the ones of the faces the layout was shaped with, in the same order. The glyphs are rasterized in them when they are not yet.
		// Bitmap: rasterized at the size rounded to a pixel, each glyph on whole units of the layout (pixels when the layout is drawn 1:1).
		// Pixel: rasterized at pixelSize, the grid of the font, the size a whole multiple of it, each glyph on whole units of the layout
		bool			Build(const TextLayout& layout, ArrayView<GlyphCache*> glyphCaches, float size, const Vector2& origin, GlyphCache::Mode mode, uint32_t pixelSize = 0);
		void			Clear();

		// A page it reads grew since it was built (or its cache was cleared): to build again. Its caches are to be alive
		bool			IsOutdated() const;
	};
}
