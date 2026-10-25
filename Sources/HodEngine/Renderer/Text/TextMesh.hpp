#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/Text/GlyphCache.hpp"

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline text
{
	class FontFace;
	struct TextLayout;
}

namespace hod::inline renderer
{
	/// @brief The quads that draw a TextLayout, one batch per GlyphCache page they read: a mesh, and a material instance with its page, per batch.
	/// Positions in the space of the layout (x right, y up), from an origin on the baseline of the start of the line.
	struct HOD_RENDERER_API TextMesh
	{
		struct Batch
		{
			uint32_t			_pageIndex = 0;
			Vector<Vector2>		_positions;
			Vector<Vector2>		_uvs;
			Vector<uint16_t>	_indices;
		};

		Vector<Batch>	_batches;

		// faces: the ones the layout was shaped with, in the same order. The glyphs are rasterized in the cache when they are not yet.
		// Bitmap: rasterized at the size rounded to a pixel
		bool			Build(const TextLayout& layout, ArrayView<const FontFace*> faces, float size, const Vector2& origin, GlyphCache& glyphCache, GlyphCache::Mode mode);
		void			Clear();
	};
}
