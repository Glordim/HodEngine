#include <gtest/gtest.h>

#include "RenderTestHelpers.hpp"

#include <HodEngine/Renderer/Text/GlyphCache.hpp>
#include <HodEngine/Renderer/Text/SkylinePacker.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Texture.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextLayout.hpp>
#include <HodEngine/Text/TextShaper.hpp>

#include <cmath>
#include <fstream>
#include <iterator>
#include <vector>

using namespace hod;

// ============================================================================
// SkylinePacker
// ============================================================================

TEST(SkylinePacker, RectanglesDoNotOverlap)
{
	struct Rectangle
	{
		uint32_t x, y, width, height;
	};
	std::vector<Rectangle> rectangles;

	SkylinePacker packer(256, 256);
	for (uint32_t index = 0; index < 64; ++index)
	{
		Rectangle rectangle = {0, 0, 8 + (index * 7) % 23, 8 + (index * 11) % 19};
		ASSERT_TRUE(packer.Pack(rectangle.width, rectangle.height, rectangle.x, rectangle.y)) << index;
		EXPECT_LE(rectangle.x + rectangle.width, 256u);
		EXPECT_LE(rectangle.y + rectangle.height, 256u);
		rectangles.push_back(rectangle);
	}

	for (size_t first = 0; first < rectangles.size(); ++first)
	{
		for (size_t second = first + 1; second < rectangles.size(); ++second)
		{
			const Rectangle& a = rectangles[first];
			const Rectangle& b = rectangles[second];
			const bool       apart = a.x + a.width <= b.x || b.x + b.width <= a.x || a.y + a.height <= b.y || b.y + b.height <= a.y;
			EXPECT_TRUE(apart) << first << " and " << second;
		}
	}
}

TEST(SkylinePacker, FillsTheAreaThenRefuses)
{
	SkylinePacker packer(64, 64);
	uint32_t      x = 0;
	uint32_t      y = 0;
	for (uint32_t index = 0; index < 4; ++index)
	{
		EXPECT_TRUE(packer.Pack(32, 32, x, y)) << index;
	}
	EXPECT_FALSE(packer.Pack(1, 1, x, y));

	SkylinePacker other(64, 64);
	EXPECT_FALSE(other.Pack(65, 1, x, y));
	EXPECT_FALSE(other.Pack(0, 1, x, y));
}

// ============================================================================
// GlyphCache, on Noto Sans
// ============================================================================

class GlyphCacheTest : public OutputCheckedTest
{
protected:
	static void SetUpTestSuite()
	{
		std::ifstream     file(std::string(TESTS_TEXT_FIXTURES_DIR) + "/Fonts/NotoSans-Regular.ttf", std::ios::binary);
		std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		_face = new FontFace();
		ASSERT_TRUE(_face->LoadFromMemory(data.data(), (uint32_t)data.size()));
	}

	static void TearDownTestSuite()
	{
		delete _face;
		_face = nullptr;
	}

	void SetUp() override
	{
		OutputCheckedTest::SetUp();
		ASSERT_TRUE(_cache.Init());
	}

	void TearDown() override
	{
		RhiDevice::GetInstance()->WaitIdle(); // the uploads to the pages
		_cache.Clear();
		OutputCheckedTest::TearDown();
	}

	uint32_t GlyphIdOf(const char* character)
	{
		const FontFace* faces[] = {_face};
		TextShaper      shaper;
		TextLayout      layout;
		EXPECT_TRUE(shaper.Shape(character, faces, 1.0f, layout));
		EXPECT_EQ(layout._glyphs.Size(), 1u);
		return layout._glyphs.Empty() ? 0 : layout._glyphs[0]._glyphId;
	}

	static float Width(const GlyphCache::Glyph& glyph)
	{
		return glyph._boundsMax.GetX() - glyph._boundsMin.GetX();
	}

protected:
	static inline FontFace* _face = nullptr;

	GlyphCache _cache;
};

TEST_F(GlyphCacheTest, SdfGlyphIsInAPage)
{
	const GlyphCache::Glyph* glyph = _cache.GetGlyph(*_face, GlyphIdOf("A"), GlyphCache::Mode::Sdf);
	ASSERT_NE(glyph, nullptr);

	EXPECT_FALSE(glyph->_empty);
	EXPECT_EQ(glyph->_pageIndex, 0u);
	ASSERT_EQ(_cache.GetPageCount(), 1u);
	EXPECT_EQ(_cache.GetPage(0)->GetFormat(), PixelFormat::R8_UNorm);

	EXPECT_GE(glyph->_uvMin.GetX(), 0.0f);
	EXPECT_GE(glyph->_uvMin.GetY(), 0.0f);
	EXPECT_LE(glyph->_uvMax.GetX(), 1.0f);
	EXPECT_LE(glyph->_uvMax.GetY(), 1.0f);
	EXPECT_LT(glyph->_uvMin.GetX(), glyph->_uvMax.GetX());
	EXPECT_LT(glyph->_uvMin.GetY(), glyph->_uvMax.GetY());

	// An A of about 0.7 em, its distance field around it, standing on the baseline
	EXPECT_GT(Width(*glyph), 0.5f);
	EXPECT_LT(Width(*glyph), 1.2f);
	EXPECT_LT(glyph->_boundsMin.GetY(), 0.0f);
	EXPECT_GT(glyph->_boundsMax.GetY(), 0.5f);
}

TEST_F(GlyphCacheTest, GlyphIsRasterizedOnce)
{
	const uint32_t           glyphId = GlyphIdOf("A");
	const GlyphCache::Glyph* first = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf);
	const GlyphCache::Glyph* second = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf);

	EXPECT_EQ(first, second);
	EXPECT_EQ(_cache.GetRasterizedGlyphCount(), 1u);
}

TEST_F(GlyphCacheTest, SpaceHasNothingToDraw)
{
	const GlyphCache::Glyph* glyph = _cache.GetGlyph(*_face, GlyphIdOf(" "), GlyphCache::Mode::Sdf);
	ASSERT_NE(glyph, nullptr);
	EXPECT_TRUE(glyph->_empty);
	EXPECT_EQ(_cache.GetPageCount(), 0u);
}

// The distance field goes SdfSpread pixels past the outline, on each side
TEST_F(GlyphCacheTest, SdfGoesPastTheOutline)
{
	const uint32_t           glyphId = GlyphIdOf("H");
	const GlyphCache::Glyph* sdf = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf);
	const GlyphCache::Glyph* bitmap = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Bitmap, GlyphCache::SdfEmSize);
	ASSERT_NE(sdf, nullptr);
	ASSERT_NE(bitmap, nullptr);

	const float spread = (float)GlyphCache::SdfSpread / (float)GlyphCache::SdfEmSize;
	const float pixel = 1.0f / (float)GlyphCache::SdfEmSize;
	EXPECT_NEAR(Width(*sdf), Width(*bitmap) + 2.0f * spread, 2.0f * pixel);
	EXPECT_NEAR(sdf->_boundsMin.GetX(), bitmap->_boundsMin.GetX() - spread, 2.0f * pixel);
	EXPECT_NEAR(sdf->_boundsMax.GetY(), bitmap->_boundsMax.GetY() + spread, 2.0f * pixel);
}

TEST_F(GlyphCacheTest, BitmapIsPerSizeSdfIsNot)
{
	const uint32_t glyphId = GlyphIdOf("A");

	const GlyphCache::Glyph* small = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Bitmap, 16);
	const GlyphCache::Glyph* large = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Bitmap, 32);
	ASSERT_NE(small, nullptr);
	ASSERT_NE(large, nullptr);
	EXPECT_NE(small, large);
	EXPECT_EQ(_cache.GetRasterizedGlyphCount(), 2u);

	// In ems, about the same glyph
	EXPECT_NEAR(Width(*small), Width(*large), 2.0f / 16.0f);

	const GlyphCache::Glyph* sdf = _cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf, 16);
	EXPECT_EQ(_cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf, 32), sdf);
	EXPECT_EQ(_cache.GetRasterizedGlyphCount(), 3u);
}

TEST_F(GlyphCacheTest, GlyphsDoNotOverlapInTheirPage)
{
	Vector<const GlyphCache::Glyph*> glyphs;
	for (const char* character : {"A", "B", "W", "g", "j", "@", "%", "Q"})
	{
		const GlyphCache::Glyph* glyph = _cache.GetGlyph(*_face, GlyphIdOf(character), GlyphCache::Mode::Sdf);
		ASSERT_NE(glyph, nullptr);
		glyphs.PushBack(glyph);
	}

	for (uint32_t first = 0; first < glyphs.Size(); ++first)
	{
		for (uint32_t second = first + 1; second < glyphs.Size(); ++second)
		{
			const GlyphCache::Glyph& a = *glyphs[first];
			const GlyphCache::Glyph& b = *glyphs[second];
			const bool apart = a._uvMax.GetX() <= b._uvMin.GetX() || b._uvMax.GetX() <= a._uvMin.GetX() || a._uvMax.GetY() <= b._uvMin.GetY() ||
			                   b._uvMax.GetY() <= a._uvMin.GetY();
			EXPECT_TRUE(apart) << first << " and " << second;
		}
	}
}

TEST_F(GlyphCacheTest, FullPageOpensAnother)
{
	for (uint32_t glyphId = 1; glyphId < 4000 && _cache.GetPageCount() < 2; ++glyphId)
	{
		ASSERT_NE(_cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf), nullptr) << glyphId;
	}
	EXPECT_EQ(_cache.GetPageCount(), 2u);
	EXPECT_EQ(_cache.GetPage(1)->GetWidth(), GlyphCache::PageSize);
}

TEST_F(GlyphCacheTest, ForgottenFaceIsRasterizedAgain)
{
	const uint32_t glyphId = GlyphIdOf("A");
	ASSERT_NE(_cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf), nullptr);

	_cache.Forget(*_face);
	ASSERT_NE(_cache.GetGlyph(*_face, glyphId, GlyphCache::Mode::Sdf), nullptr);
	EXPECT_EQ(_cache.GetRasterizedGlyphCount(), 2u);
}

TEST_F(GlyphCacheTest, BitmapNeedsASize)
{
	EXPECT_EQ(_cache.GetGlyph(*_face, GlyphIdOf("A"), GlyphCache::Mode::Bitmap), nullptr);

	FontFace notLoaded;
	EXPECT_EQ(_cache.GetGlyph(notLoaded, 1, GlyphCache::Mode::Sdf), nullptr);

	_expectedErrorCount = 2;
}
