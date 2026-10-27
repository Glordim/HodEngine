#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontThumbnail.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Renderer/Text/TextRasterizer.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/SampleText.hpp>

#include <cmath>
#include <limits>

namespace hod::inline editor
{
	namespace
	{
		constexpr float   InkRatio = 0.6f;    // of the thumbnail the ink covers, on its largest side
		constexpr float   LineGapRatio = 0.08f; // of the thumbnail between two lines
		constexpr uint8_t BackgroundColor = 40;
		constexpr uint8_t TextColor = 235;
	}

	/// @brief
	/// @param face
	/// @param renderMode
	/// @param pixelSize
	/// @param rgbaPixels
	/// @return
	bool FontThumbnail::Generate(const FontFace& face, FontFaceResource::RenderMode renderMode, uint32_t pixelSize, Vector<uint8_t>& rgbaPixels)
	{
		Line line;
		line._face = &face;
		line._renderMode = renderMode;
		line._pixelSize = pixelSize;
		return Generate({line}, rgbaPixels);
	}

	/// @brief Each line rasterized once to measure it, then all at the size that fits them in the thumbnail (Pixel Art: the largest whole multiple
	/// of its grid that does): their weights compare
	/// @param lines
	/// @param rgbaPixels
	/// @return
	bool FontThumbnail::Generate(const Vector<Line>& lines, Vector<uint8_t>& rgbaPixels)
	{
		if (lines.Empty())
		{
			return false;
		}

		const String sampleText = GetSampleText(*lines[0]._face); // "Aa", or what the font is for: the same text on each line
		if (sampleText.Empty())
		{
			return false;
		}
		const char* text = sampleText.CStr();

		const uint32_t lineCount = lines.Size();
		const float    inkSize = InkRatio * (float)Size;
		const float    lineGap = lineCount > 1 ? LineGapRatio * (float)Size : 0.0f;
		const float    lineHeight = (inkSize - lineGap * (float)(lineCount - 1)) / (float)lineCount;

		// The size each line fits at, the smallest of them for all
		constexpr float measureSize = 100.0f;
		float           fontSize = std::numeric_limits<float>::max();
		for (const Line& line : lines)
		{
			const bool            pixelArt = line._renderMode == FontFaceResource::RenderMode::PixelArt && line._pixelSize != 0;
			const float           measuredSize = pixelArt ? (float)line._pixelSize : measureSize;
			TextRasterizer::Image image;
			if (TextRasterizer::Rasterize(*line._face, text, measuredSize, pixelArt ? GlyphCache::Mode::Pixel : GlyphCache::Mode::Bitmap, line._pixelSize, image) == false)
			{
				return false;
			}
			fontSize = Min(fontSize, measuredSize * Min(inkSize / (float)image._width, (lineCount > 1 ? lineHeight : inkSize) / (float)image._height));
		}

		Vector<TextRasterizer::Image> images;
		uint32_t                      totalHeight = 0;
		for (const Line& line : lines)
		{
			const bool pixelArt = line._renderMode == FontFaceResource::RenderMode::PixelArt && line._pixelSize != 0;
			const float lineFontSize = pixelArt ? Max(1.0f, std::floor(fontSize / (float)line._pixelSize)) * (float)line._pixelSize : fontSize;

			TextRasterizer::Image& image = images.EmplaceBack();
			if (TextRasterizer::Rasterize(*line._face, text, lineFontSize, pixelArt ? GlyphCache::Mode::Pixel : GlyphCache::Mode::Bitmap, line._pixelSize, image) == false ||
			    image._width > Size || image._height > Size)
			{
				return false;
			}
			totalHeight += image._height;
		}
		totalHeight += (uint32_t)lineGap * (lineCount - 1);
		if (totalHeight > Size)
		{
			return false;
		}

		rgbaPixels.Resize(Size * Size * 4);
		for (uint32_t pixelIndex = 0; pixelIndex < Size * Size; ++pixelIndex)
		{
			uint8_t* pixel = rgbaPixels.Data() + pixelIndex * 4;
			pixel[0] = BackgroundColor;
			pixel[1] = BackgroundColor;
			pixel[2] = BackgroundColor;
			pixel[3] = 255;
		}

		// The lines one under the other, each centered, light on the background
		uint32_t top = (Size - totalHeight) / 2;
		for (const TextRasterizer::Image& image : images)
		{
			const uint32_t left = (Size - image._width) / 2;
			for (uint32_t y = 0; y < image._height; ++y)
			{
				for (uint32_t x = 0; x < image._width; ++x)
				{
					const uint8_t coverage = image._coverage[y * image._width + x];
					const uint8_t value = (uint8_t)((BackgroundColor * (255 - coverage) + TextColor * coverage) / 255);
					uint8_t*      pixel = rgbaPixels.Data() + ((top + y) * Size + left + x) * 4;
					pixel[0] = value;
					pixel[1] = value;
					pixel[2] = value;
				}
			}
			top += image._height + (uint32_t)lineGap;
		}
		return true;
	}
}
