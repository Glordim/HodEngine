#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontThumbnail.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Renderer/Text/TextRasterizer.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/SampleText.hpp>

#include <cmath>

namespace hod::inline editor
{
	namespace
	{
		constexpr float  InkRatio = 0.6f; // of the thumbnail the ink covers, on its largest side
		constexpr uint8_t BackgroundColor = 40;
		constexpr uint8_t TextColor = 235;
	}

	/// @brief Rasterized once to measure it, then at the size that fills the thumbnail (Pixel Art: the largest whole multiple of its grid that does)
	/// @param face
	/// @param renderMode
	/// @param pixelSize
	/// @param rgbaPixels
	/// @return
	bool FontThumbnail::Generate(const FontFace& face, FontResource::RenderMode renderMode, uint32_t pixelSize, Vector<uint8_t>& rgbaPixels)
	{
		const String sampleText = GetSampleText(face); // "Aa", or what the font is for
		if (sampleText.Empty())
		{
			return false;
		}
		const char* text = sampleText.CStr();

		const bool             pixelArt = renderMode == FontResource::RenderMode::PixelArt && pixelSize != 0;
		const GlyphCache::Mode mode = pixelArt ? GlyphCache::Mode::Pixel : GlyphCache::Mode::Bitmap;

		constexpr float       measureSize = 100.0f;
		TextRasterizer::Image image;
		if (TextRasterizer::Rasterize(face, text, pixelArt ? (float)pixelSize : measureSize, mode, pixelSize, image) == false)
		{
			return false;
		}

		const float fill = InkRatio * (float)Size / (float)Max(image._width, image._height);
		const float measuredSize = pixelArt ? (float)pixelSize : measureSize;
		float       fontSize = measuredSize * fill;
		if (pixelArt)
		{
			fontSize = Max(1.0f, std::floor(fontSize / (float)pixelSize)) * (float)pixelSize;
		}
		if (TextRasterizer::Rasterize(face, text, fontSize, mode, pixelSize, image) == false || image._width > Size || image._height > Size)
		{
			return false;
		}

		// The ink in the middle, light on the background
		rgbaPixels.Resize(Size * Size * 4);
		const uint32_t left = (Size - image._width) / 2;
		const uint32_t top = (Size - image._height) / 2;
		for (uint32_t y = 0; y < Size; ++y)
		{
			for (uint32_t x = 0; x < Size; ++x)
			{
				uint8_t coverage = 0;
				if (x >= left && x < left + image._width && y >= top && y < top + image._height)
				{
					coverage = image._coverage[(y - top) * image._width + (x - left)];
				}

				const uint8_t value = (uint8_t)((BackgroundColor * (255 - coverage) + TextColor * coverage) / 255);
				uint8_t*      pixel = rgbaPixels.Data() + (y * Size + x) * 4;
				pixel[0] = value;
				pixel[1] = value;
				pixel[2] = value;
				pixel[3] = 255;
			}
		}
		return true;
	}
}
