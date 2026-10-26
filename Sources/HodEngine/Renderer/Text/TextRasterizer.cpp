#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/TextRasterizer.hpp"
#include "HodEngine/Renderer/Text/FreeTypeGlyph.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextLayout.hpp>
#include <HodEngine/Text/TextShaper.hpp>

#include <cmath>
#include <limits>

namespace hod::inline renderer
{
	namespace
	{
		// A glyph rasterized, where it goes in the line (y up, from the baseline)
		struct PlacedGlyph
		{
			int32_t			_left = 0;
			int32_t			_top = 0;
			uint32_t		_width = 0;
			uint32_t		_height = 0;
			Vector<uint8_t>	_coverage;
		};
	}

	/// @brief Its own FreeType library and shaper: any thread
	/// @param face
	/// @param text
	/// @param size
	/// @param mode
	/// @param pixelSize
	/// @param image
	/// @return
	bool TextRasterizer::Rasterize(const FontFace& face, StringView text, float size, GlyphCache::Mode mode, uint32_t pixelSize, Image& image)
	{
		image = Image();

		if (face.IsLoaded() == false || (mode != GlyphCache::Mode::Bitmap && mode != GlyphCache::Mode::Pixel) || (mode == GlyphCache::Mode::Pixel && pixelSize == 0))
		{
			OUTPUT_ERROR("TextRasterizer::Rasterize: a loaded face, Bitmap or Pixel with its grid");
			return false;
		}

		const FontFace* faces[] = {&face};
		TextShaper      shaper;
		TextLayout      layout;
		if (shaper.Shape(text, faces, size, layout) == false)
		{
			return false;
		}

		// Pixel: rasterized on the grid, each of its pixels scale x scale pixels
		const uint32_t rasterSize = (mode == GlyphCache::Mode::Pixel) ? pixelSize : (uint32_t)Max(1.0f, std::round(size));
		const uint32_t scale = (mode == GlyphCache::Mode::Pixel) ? Max(1u, (uint32_t)std::round(size / (float)pixelSize)) : 1;

		FT_Library ftLibrary = nullptr;
		if (FT_Init_FreeType(&ftLibrary) != 0)
		{
			OUTPUT_ERROR("TextRasterizer: FT_Init_FreeType failed");
			return false;
		}
		FT_Face ftFace = nullptr;
		if (FT_New_Memory_Face(ftLibrary, face.GetData(), (FT_Long)face.GetDataSize(), (FT_Long)face.GetFaceIndex(), &ftFace) != 0 ||
		    FT_Set_Pixel_Sizes(ftFace, 0, rasterSize) != 0)
		{
			OUTPUT_ERROR("TextRasterizer: FreeType can't read the face");
			if (ftFace != nullptr)
			{
				FT_Done_Face(ftFace);
			}
			FT_Done_FreeType(ftLibrary);
			return false;
		}

		FT_Int32       loadFlags = 0;
		FT_Render_Mode renderMode = FT_RENDER_MODE_NORMAL;
		freetype::GetFlags(mode, loadFlags, renderMode);

		Vector<PlacedGlyph> placedGlyphs;
		int32_t             left = std::numeric_limits<int32_t>::max();
		int32_t             right = std::numeric_limits<int32_t>::min();
		int32_t             top = std::numeric_limits<int32_t>::min();
		int32_t             bottom = std::numeric_limits<int32_t>::max();

		for (const TextLayout::Glyph& layoutGlyph : layout._glyphs)
		{
			if (FT_Load_Glyph(ftFace, layoutGlyph._glyphId, loadFlags) != 0 || FT_Render_Glyph(ftFace->glyph, renderMode) != 0)
			{
				continue;
			}
			const FT_Bitmap& bitmap = ftFace->glyph->bitmap;
			if (bitmap.width == 0 || bitmap.rows == 0 || (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY && bitmap.pixel_mode != FT_PIXEL_MODE_MONO))
			{
				continue;
			}

			// On whole pixels, as TextMesh places them
			PlacedGlyph& placedGlyph = placedGlyphs.EmplaceBack();
			placedGlyph._left = (int32_t)std::round(layoutGlyph._position.GetX()) + ftFace->glyph->bitmap_left * (int32_t)scale;
			placedGlyph._top = (int32_t)std::round(layoutGlyph._position.GetY()) + ftFace->glyph->bitmap_top * (int32_t)scale;
			placedGlyph._width = bitmap.width;
			placedGlyph._height = bitmap.rows;
			placedGlyph._coverage.Resize(bitmap.width * bitmap.rows);
			for (uint32_t row = 0; row < bitmap.rows; ++row)
			{
				freetype::CopyRow(bitmap, row, placedGlyph._coverage.Data() + row * bitmap.width);
			}

			left = Min(left, placedGlyph._left);
			right = Max(right, placedGlyph._left + (int32_t)(bitmap.width * scale));
			top = Max(top, placedGlyph._top);
			bottom = Min(bottom, placedGlyph._top - (int32_t)(bitmap.rows * scale));
		}

		FT_Done_Face(ftFace);
		FT_Done_FreeType(ftLibrary);

		if (placedGlyphs.Empty())
		{
			return false;
		}

		image._width = (uint32_t)(right - left);
		image._height = (uint32_t)(top - bottom);
		image._coverage.Resize(image._width * image._height, 0);

		// The coverages together: the largest where glyphs overlap (kerning, marks)
		for (const PlacedGlyph& placedGlyph : placedGlyphs)
		{
			for (uint32_t y = 0; y < placedGlyph._height * scale; ++y)
			{
				const uint32_t imageRow = (uint32_t)(top - placedGlyph._top) + y;
				const uint8_t* glyphRow = placedGlyph._coverage.Data() + (y / scale) * placedGlyph._width;
				uint8_t*       pixel = image._coverage.Data() + imageRow * image._width + (uint32_t)(placedGlyph._left - left);
				for (uint32_t x = 0; x < placedGlyph._width * scale; ++x)
				{
					pixel[x] = Max(pixel[x], glyphRow[x / scale]);
				}
			}
		}

		return true;
	}
}
