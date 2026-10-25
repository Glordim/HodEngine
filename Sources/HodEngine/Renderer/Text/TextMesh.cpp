#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/TextMesh.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextLayout.hpp>

#include <cmath>
#include <limits>

namespace hod::inline renderer
{
	/// @brief
	void TextMesh::Clear()
	{
		_batches.Clear();
	}

	/// @brief
	/// @param layout
	/// @param faces
	/// @param size
	/// @param origin
	/// @param glyphCache
	/// @param mode
	/// @return false when a glyph can't be rasterized: the others are drawn all the same
	bool TextMesh::Build(const TextLayout& layout, ArrayView<const FontFace*> faces, float size, const Vector2& origin, GlyphCache& glyphCache, GlyphCache::Mode mode)
	{
		Clear();

		const uint32_t pixelSize = (mode == GlyphCache::Mode::Bitmap) ? (uint32_t)Max(1.0f, std::round(size)) : 0;

		bool built = true;
		for (const TextLayout::Glyph& layoutGlyph : layout._glyphs)
		{
			if (layoutGlyph._faceIndex >= faces.Size() || faces[layoutGlyph._faceIndex] == nullptr)
			{
				built = false;
				continue;
			}

			const GlyphCache::Glyph* glyph = glyphCache.GetGlyph(*faces[layoutGlyph._faceIndex], layoutGlyph._glyphId, mode, pixelSize);
			if (glyph == nullptr)
			{
				built = false;
				continue;
			}
			if (glyph->_empty)
			{
				continue;
			}

			Batch* batch = nullptr;
			for (Batch& candidate : _batches)
			{
				if (candidate._pageIndex == glyph->_pageIndex)
				{
					batch = &candidate;
					break;
				}
			}
			if (batch == nullptr)
			{
				batch = &_batches.EmplaceBack();
				batch->_pageIndex = glyph->_pageIndex;
			}

			if (batch->_positions.Size() + 4 > std::numeric_limits<uint16_t>::max())
			{
				built = false; // 16 bits indices: that many glyphs in a page is not drawn
				continue;
			}

			const Vector2  glyphOrigin = origin + layoutGlyph._position;
			const Vector2  quadMin = glyphOrigin + glyph->_boundsMin * size;
			const Vector2  quadMax = glyphOrigin + glyph->_boundsMax * size;
			const uint16_t vertexIndex = (uint16_t)batch->_positions.Size();

			// The top of the glyph is the top of its texels: v goes down
			batch->_positions.PushBack(Vector2(quadMin.GetX(), quadMax.GetY()));
			batch->_positions.PushBack(Vector2(quadMax.GetX(), quadMax.GetY()));
			batch->_positions.PushBack(Vector2(quadMin.GetX(), quadMin.GetY()));
			batch->_positions.PushBack(Vector2(quadMax.GetX(), quadMin.GetY()));

			batch->_uvs.PushBack(glyph->_uvMin);
			batch->_uvs.PushBack(Vector2(glyph->_uvMax.GetX(), glyph->_uvMin.GetY()));
			batch->_uvs.PushBack(Vector2(glyph->_uvMin.GetX(), glyph->_uvMax.GetY()));
			batch->_uvs.PushBack(glyph->_uvMax);

			// Counter-clockwise, y up
			batch->_indices.PushBack((uint16_t)(vertexIndex + 1));
			batch->_indices.PushBack((uint16_t)(vertexIndex + 0));
			batch->_indices.PushBack((uint16_t)(vertexIndex + 2));
			batch->_indices.PushBack((uint16_t)(vertexIndex + 1));
			batch->_indices.PushBack((uint16_t)(vertexIndex + 2));
			batch->_indices.PushBack((uint16_t)(vertexIndex + 3));
		}

		return built;
	}
}
