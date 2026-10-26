#pragma once

#include "HodEngine/Renderer/Text/GlyphCache.hpp"

#include <freetype/freetype.h>

#include <cstdint>

// Internal to the Renderer: how FreeType rasterizes a glyph for a mode of GlyphCache, shared by GlyphCache and TextRasterizer
namespace hod::inline renderer::freetype
{
	/// @brief No hinting for SDF: it is scaled, nor for Pixel: its outlines are on its grid already.
	/// Light hinting for Bitmap: vertical only, the advances are the ones of the shaper
	inline void GetFlags(GlyphCache::Mode mode, FT_Int32& loadFlags, FT_Render_Mode& renderMode)
	{
		switch (mode)
		{
			case GlyphCache::Mode::Bitmap:
				loadFlags = FT_LOAD_TARGET_LIGHT;
				renderMode = FT_RENDER_MODE_NORMAL;
				break;
			case GlyphCache::Mode::Pixel:
				loadFlags = FT_LOAD_NO_HINTING | FT_LOAD_TARGET_MONO;
				renderMode = FT_RENDER_MODE_MONO;
				break;
			default:
				loadFlags = FT_LOAD_NO_HINTING;
				renderMode = FT_RENDER_MODE_SDF;
				break;
		}
	}

	/// @brief A row of a rasterized glyph as 8 bits of coverage.
	/// The rows of the bitmap may be further apart, or go up: the pitch goes down one row.
	/// 1 bit (Pixel): 8 pixels a byte, the leftmost in the highest bit, on is full coverage
	inline void CopyRow(const FT_Bitmap& bitmap, uint32_t row, uint8_t* destination)
	{
		const uint8_t* bitmapRow = bitmap.buffer + (ptrdiff_t)row * bitmap.pitch;
		if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
		{
			for (uint32_t column = 0; column < bitmap.width; ++column)
			{
				destination[column] = ((bitmapRow[column / 8] >> (7 - column % 8)) & 1) ? 255 : 0;
			}
		}
		else
		{
			for (uint32_t column = 0; column < bitmap.width; ++column)
			{
				destination[column] = bitmapRow[column];
			}
		}
	}
}
