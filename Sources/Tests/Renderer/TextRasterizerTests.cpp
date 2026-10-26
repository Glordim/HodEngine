#include <gtest/gtest.h>

#include "RenderTestHelpers.hpp"

#include <HodEngine/Renderer/Text/TextRasterizer.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <fstream>
#include <iterator>
#include <vector>

using namespace hod;

// ============================================================================
// TextRasterizer: a line on the CPU (thumbnails)
// ============================================================================

class TextRasterizerTest : public OutputCheckedTest
{
protected:
	static FontFace* LoadFace(const char* name)
	{
		std::ifstream     file(std::string(TESTS_TEXT_FIXTURES_DIR) + "/Fonts/" + name, std::ios::binary);
		std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		FontFace*         face = new FontFace();
		EXPECT_TRUE(face->LoadFromMemory(data.data(), (uint32_t)data.size()));
		return face;
	}
};

// The ink of "Aa": as high as the capitals, anti-aliased
TEST_F(TextRasterizerTest, BitmapLineIsItsInk)
{
	FontFace*             face = LoadFace("NotoSans-Regular.ttf");
	TextRasterizer::Image image;
	ASSERT_TRUE(TextRasterizer::Rasterize(*face, "Aa", 100.0f, GlyphCache::Mode::Bitmap, 0, image));

	const float capHeight = 100.0f * (float)face->GetCapHeight() / (float)face->GetUnitsPerEm();
	EXPECT_NEAR((float)image._height, capHeight, 3.0f);
	EXPECT_GT(image._width, image._height);
	ASSERT_EQ(image._coverage.Size(), image._width * image._height);

	bool full = false;
	bool partial = false;
	for (uint8_t coverage : image._coverage)
	{
		full |= coverage == 255;
		partial |= coverage > 0 && coverage < 255;
	}
	EXPECT_TRUE(full);
	EXPECT_TRUE(partial);

	delete face;
}

// Silkscreen at 4 times its grid: every pixel on or off, the glyphs in blocks of 4 x 4
TEST_F(TextRasterizerTest, PixelLineIsOnItsGrid)
{
	FontFace*             face = LoadFace("Silkscreen-Regular.ttf");
	TextRasterizer::Image image;
	ASSERT_TRUE(TextRasterizer::Rasterize(*face, "Aa", 32.0f, GlyphCache::Mode::Pixel, 8, image));

	EXPECT_EQ(image._width % 4, 0u);
	EXPECT_EQ(image._height % 4, 0u);
	for (uint8_t coverage : image._coverage)
	{
		EXPECT_TRUE(coverage == 0 || coverage == 255);
	}

	delete face;
}

TEST_F(TextRasterizerTest, PixelNeedsItsGrid)
{
	FontFace*             face = LoadFace("Silkscreen-Regular.ttf");
	TextRasterizer::Image image;
	EXPECT_FALSE(TextRasterizer::Rasterize(*face, "Aa", 32.0f, GlyphCache::Mode::Pixel, 0, image));
	_expectedErrorCount = 1;

	delete face;
}
