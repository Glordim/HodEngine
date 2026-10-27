#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/TextMesh.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Text/TextLayout.hpp>

#include <cmath>
#include <limits>

namespace hod::inline renderer
{
	/// @brief
	void TextMesh::Clear()
	{
		_batches.Clear();
		_inkMin = Vector2(0.0f, 0.0f);
		_inkMax = Vector2(0.0f, 0.0f);
	}

	/// @brief
	/// @return
	bool TextMesh::HasInk() const
	{
		return _inkMax.GetX() > _inkMin.GetX() && _inkMax.GetY() > _inkMin.GetY();
	}

	/// @brief The caches it is built from are still alive: the ones of the fonts the text is drawn with
	/// @return
	bool TextMesh::IsOutdated() const
	{
		for (const Batch& batch : _batches)
		{
			if (batch._glyphCache->GetGeneration() != batch._glyphCacheGeneration)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief The glyphs are asked for first, all of them: a page that grows for one changes the uvs of the ones before it
	/// @param layout
	/// @param fonts
	/// @param size
	/// @param origin
	/// @return false when a glyph can't be rasterized: the others are drawn all the same
	bool TextMesh::Build(const TextLayout& layout, ArrayView<Font> fonts, float size, const Vector2& origin)
	{
		Clear();

		const uint32_t bitmapPixelSize = (uint32_t)Max(1.0f, std::round(size));
		auto           getPixelSize = [bitmapPixelSize](const Font& font)
		{
			return font._mode == GlyphCache::Mode::Bitmap ? bitmapPixelSize : font._pixelSize;
		};

		bool                             built = true;
		Vector<const GlyphCache::Glyph*> glyphs;
		glyphs.Resize(layout._glyphs.Size(), nullptr);
		for (uint32_t glyphIndex = 0; glyphIndex < layout._glyphs.Size(); ++glyphIndex)
		{
			const TextLayout::Glyph& layoutGlyph = layout._glyphs[glyphIndex];
			if (layoutGlyph._faceIndex >= fonts.Size() || fonts[layoutGlyph._faceIndex]._glyphCache == nullptr)
			{
				built = false;
				continue;
			}

			const Font& font = fonts[layoutGlyph._faceIndex];
			glyphs[glyphIndex] = font._glyphCache->GetGlyph(layoutGlyph._glyphId, font._mode, getPixelSize(font));
			if (glyphs[glyphIndex] == nullptr)
			{
				built = false;
			}
		}

		bool firstInk = true;
		for (uint32_t glyphIndex = 0; glyphIndex < layout._glyphs.Size(); ++glyphIndex)
		{
			const TextLayout::Glyph& layoutGlyph = layout._glyphs[glyphIndex];
			const GlyphCache::Glyph* glyph = glyphs[glyphIndex];
			if (glyph == nullptr || glyph->_empty)
			{
				continue;
			}

			const Font&      font = fonts[layoutGlyph._faceIndex];
			GlyphCache*      glyphCache = font._glyphCache;
			GlyphCache::Mode mode = font._mode;
			Batch*           batch = nullptr;
			for (Batch& candidate : _batches)
			{
				if (candidate._glyphCache == glyphCache && candidate._mode == mode && candidate._pageIndex == glyph->_pageIndex)
				{
					batch = &candidate;
					break;
				}
			}
			if (batch == nullptr)
			{
				batch = &_batches.EmplaceBack();
				batch->_glyphCache = glyphCache;
				batch->_glyphCacheGeneration = glyphCache->GetGeneration();
				batch->_mode = mode;
				batch->_pageIndex = glyph->_pageIndex;
			}

			if (batch->_positions.Size() + 4 > std::numeric_limits<uint16_t>::max())
			{
				built = false; // 16 bits indices: that many glyphs in a page is not drawn
				continue;
			}

			// Bitmap: the texels of the glyph on whole pixels, at the size they are rasterized at, not stretched nor filtered across two pixels.
			// Pixel: on whole pixels too, its texels as large as the size is a multiple of the grid
			Vector2 glyphOrigin = origin + layoutGlyph._position;
			float   quadScale = size;
			if (mode == GlyphCache::Mode::Bitmap || mode == GlyphCache::Mode::Pixel)
			{
				glyphOrigin = Vector2(std::round(glyphOrigin.GetX()), std::round(glyphOrigin.GetY()));
			}
			if (mode == GlyphCache::Mode::Bitmap)
			{
				quadScale = (float)bitmapPixelSize;
			}
			const Vector2 quadMin = glyphOrigin + glyph->_boundsMin * quadScale;
			const Vector2 quadMax = glyphOrigin + glyph->_boundsMax * quadScale;

			const Vector2 inkMin = glyphOrigin + glyph->_inkMin * quadScale;
			const Vector2 inkMax = glyphOrigin + glyph->_inkMax * quadScale;
			if (firstInk)
			{
				firstInk = false;
				_inkMin = inkMin;
				_inkMax = inkMax;
			}
			else
			{
				_inkMin = Vector2(Min(_inkMin.GetX(), inkMin.GetX()), Min(_inkMin.GetY(), inkMin.GetY()));
				_inkMax = Vector2(Max(_inkMax.GetX(), inkMax.GetX()), Max(_inkMax.GetY(), inkMax.GetY()));
			}
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
