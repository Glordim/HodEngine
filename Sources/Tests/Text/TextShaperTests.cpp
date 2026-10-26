#include <gtest/gtest.h>

#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextLayout.hpp>
#include <HodEngine/Text/TextShaper.hpp>

#include <cmath>
#include <fstream>
#include <iterator>
#include <vector>

using namespace hod;

namespace
{
	std::vector<char> ReadFixture(const char* name)
	{
		std::ifstream file(std::string(TESTS_TEXT_FIXTURES_DIR) + "/Fonts/" + name, std::ios::binary);
		return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}

	constexpr float Size = 32.0f;
}

// ============================================================================
// Noto Sans, then Noto Sans SC to fall back to for the CJK characters it lacks.
// Loaded once: Noto Sans SC is 10 MB.
// ============================================================================

class TextShaperTest : public ::testing::Test
{
protected:
	static void SetUpTestSuite()
	{
		_sans = new FontFace();
		std::vector<char> sansData = ReadFixture("NotoSans-Regular.ttf");
		ASSERT_FALSE(sansData.empty());
		ASSERT_TRUE(_sans->LoadFromMemory(sansData.data(), (uint32_t)sansData.size()));

		_sc = new FontFace();
		std::vector<char> scData = ReadFixture("NotoSansSC-Regular.ttf");
		ASSERT_FALSE(scData.empty());
		ASSERT_TRUE(_sc->LoadFromMemory(scData.data(), (uint32_t)scData.size()));
	}

	static void TearDownTestSuite()
	{
		delete _sans;
		_sans = nullptr;
		delete _sc;
		_sc = nullptr;
	}

	TextLayout Shape(const char* text, float size = Size)
	{
		const FontFace* faces[] = {_sans, _sc};
		TextLayout      layout;
		EXPECT_TRUE(_shaper.Shape(text, faces, size, layout));
		return layout;
	}

	TextLayout ShapeWithoutFallback(const char* text)
	{
		const FontFace* faces[] = {_sans};
		TextLayout      layout;
		EXPECT_TRUE(_shaper.Shape(text, faces, Size, layout));
		return layout;
	}

protected:
	static inline FontFace* _sans = nullptr;
	static inline FontFace* _sc = nullptr;

	TextShaper _shaper;
};

// ============================================================================
// FontFace
// ============================================================================

TEST_F(TextShaperTest, FaceHasItsMetrics)
{
	EXPECT_TRUE(_sans->IsLoaded());
	EXPECT_EQ(_sans->GetUnitsPerEm(), 1000u);
	EXPECT_GT(_sans->GetAscender(), 0);
	EXPECT_LT(_sans->GetDescender(), 0);
	EXPECT_EQ(_sans->GetDataSize(), (uint32_t)ReadFixture("NotoSans-Regular.ttf").size());

	// The flat capitals are below the ascender, which leaves room for their accents
	EXPECT_GT(_sans->GetCapHeight(), 600);
	EXPECT_LT(_sans->GetCapHeight(), _sans->GetAscender());
}

TEST_F(TextShaperTest, FaceKnowsTheCharactersItHas)
{
	EXPECT_TRUE(_sans->HasGlyph(U'A'));
	EXPECT_TRUE(_sans->HasGlyph(0x00E9)); // é
	EXPECT_FALSE(_sans->HasGlyph(0x4E2D)); // 中
	EXPECT_TRUE(_sc->HasGlyph(0x4E2D));
}

TEST(FontFace, RejectsWhatIsNotAFont)
{
	const char garbage[64] = "not a font, not even close to the size of one";

	FontFace face;
	EXPECT_FALSE(face.LoadFromMemory(garbage, sizeof(garbage)));
	EXPECT_FALSE(face.IsLoaded());
	EXPECT_FALSE(face.HasGlyph(U'A'));
}

// ============================================================================
// Shaping
// ============================================================================

TEST_F(TextShaperTest, EmptyTextHasTheMetricsOfTheFirstFace)
{
	TextLayout layout = Shape("");

	EXPECT_TRUE(layout._glyphs.Empty());
	EXPECT_EQ(layout._width, 0.0f);
	EXPECT_FLOAT_EQ(layout._ascender, _sans->GetAscender() * Size / 1000.0f);
	EXPECT_FLOAT_EQ(layout._descender, _sans->GetDescender() * Size / 1000.0f);
	EXPECT_GT(layout.GetLineHeight(), 0.0f);
}

TEST_F(TextShaperTest, GlyphsAreOneAfterTheOther)
{
	TextLayout layout = Shape("Hello");

	ASSERT_EQ(layout._glyphs.Size(), 5u);
	float penX = 0.0f;
	for (uint32_t glyphIndex = 0; glyphIndex < layout._glyphs.Size(); ++glyphIndex)
	{
		const TextLayout::Glyph& glyph = layout._glyphs[glyphIndex];
		EXPECT_NE(glyph._glyphId, 0u);
		EXPECT_EQ(glyph._faceIndex, 0u);
		EXPECT_EQ(glyph._cluster, glyphIndex);
		EXPECT_FLOAT_EQ(glyph._position.GetX(), penX);
		EXPECT_FLOAT_EQ(glyph._position.GetY(), 0.0f);
		EXPECT_GT(glyph._advance, 0.0f);
		penX += glyph._advance;
	}
	EXPECT_FLOAT_EQ(layout._width, penX);
}

TEST_F(TextShaperTest, ScalesWithTheSize)
{
	TextLayout small = Shape("Hello", 16.0f);
	TextLayout large = Shape("Hello", 32.0f);

	EXPECT_FLOAT_EQ(large._width, small._width * 2.0f);
	EXPECT_FLOAT_EQ(large._ascender, small._ascender * 2.0f);
}

// The kerning of the font brings V under the arm of A
TEST_F(TextShaperTest, Kerning)
{
	const float pairWidth = Shape("AV")._width;
	const float apartWidth = Shape("A")._width + Shape("V")._width;

	EXPECT_LT(pairWidth, apartWidth);
}

TEST_F(TextShaperTest, Ligature)
{
	TextLayout layout = Shape("fi");

	ASSERT_EQ(layout._glyphs.Size(), 1u);
	EXPECT_EQ(layout._glyphs[0]._cluster, 0u);
}

TEST_F(TextShaperTest, ClustersAreByteOffsets)
{
	TextLayout layout = Shape("\xC3\xA9" "a"); // é: 2 bytes

	ASSERT_EQ(layout._glyphs.Size(), 2u);
	EXPECT_EQ(layout._glyphs[0]._cluster, 0u);
	EXPECT_EQ(layout._glyphs[1]._cluster, 2u);
}

// A letter and its combining mark stay one cluster
TEST_F(TextShaperTest, CombiningMarkStaysWithItsLetter)
{
	TextLayout layout = Shape("e\xCC\x81" "a"); // e + U+0301

	for (const TextLayout::Glyph& glyph : layout._glyphs)
	{
		EXPECT_NE(glyph._glyphId, 0u);
		EXPECT_TRUE(glyph._cluster == 0u || glyph._cluster == 3u);
	}
	EXPECT_EQ(layout._glyphs.Back()._cluster, 3u);
}

TEST_F(TextShaperTest, InvalidUtf8IsOneCharacter)
{
	TextLayout layout = Shape("a\xFF" "b");

	ASSERT_EQ(layout._glyphs.Size(), 3u);
	EXPECT_EQ(layout._glyphs[0]._cluster, 0u);
	EXPECT_EQ(layout._glyphs[1]._cluster, 1u);
	EXPECT_EQ(layout._glyphs[2]._cluster, 2u);
}

// ============================================================================
// Fallback
// ============================================================================

TEST_F(TextShaperTest, FallsBackForWhatTheFirstFaceLacks)
{
	TextLayout layout = Shape("A\xE4\xB8\xAD" "B"); // A 中 B

	ASSERT_EQ(layout._glyphs.Size(), 3u);
	EXPECT_EQ(layout._glyphs[0]._faceIndex, 0u);
	EXPECT_EQ(layout._glyphs[1]._faceIndex, 1u);
	EXPECT_EQ(layout._glyphs[2]._faceIndex, 0u);

	EXPECT_EQ(layout._glyphs[0]._cluster, 0u);
	EXPECT_EQ(layout._glyphs[1]._cluster, 1u);
	EXPECT_EQ(layout._glyphs[2]._cluster, 4u);

	float penX = 0.0f;
	for (const TextLayout::Glyph& glyph : layout._glyphs)
	{
		EXPECT_NE(glyph._glyphId, 0u);
		EXPECT_FLOAT_EQ(glyph._position.GetX(), penX);
		penX += glyph._advance;
	}
}

TEST_F(TextShaperTest, LineFitsEveryFaceItUses)
{
	TextLayout latin = Shape("AB");
	TextLayout mixed = Shape("A\xE4\xB8\xAD");

	EXPECT_FLOAT_EQ(latin._ascender, _sans->GetAscender() * Size / 1000.0f);
	EXPECT_GE(mixed._ascender, latin._ascender);
	EXPECT_GE(mixed._ascender, _sc->GetAscender() * Size / _sc->GetUnitsPerEm());
	EXPECT_LE(mixed._descender, _sc->GetDescender() * Size / _sc->GetUnitsPerEm());
}

TEST_F(TextShaperTest, WithoutFallbackTheMissingGlyphIsDrawn)
{
	TextLayout layout = ShapeWithoutFallback("A\xE4\xB8\xAD" "B");

	ASSERT_EQ(layout._glyphs.Size(), 3u);
	EXPECT_EQ(layout._glyphs[1]._glyphId, 0u);
	EXPECT_EQ(layout._glyphs[1]._faceIndex, 0u);
	EXPECT_GT(layout._glyphs[1]._advance, 0.0f);
}

TEST_F(TextShaperTest, NoneHasItIsTheMissingGlyphOfTheFirstFace)
{
	TextLayout layout = Shape("A\xF0\x9F\x98\x80"); // A, U+1F600, an emoji neither has

	ASSERT_EQ(layout._glyphs.Size(), 2u);
	EXPECT_EQ(layout._glyphs[1]._glyphId, 0u);
	EXPECT_EQ(layout._glyphs[1]._faceIndex, 0u);
	EXPECT_EQ(layout._glyphs[1]._cluster, 1u);
}

TEST_F(TextShaperTest, RejectsWhatItCannotShapeWith)
{
	TextLayout layout;
	EXPECT_FALSE(_shaper.Shape("A", ArrayView<const FontFace*>(), Size, layout));

	const FontFace* faces[] = {_sans};
	EXPECT_FALSE(_shaper.Shape("A", faces, 0.0f, layout));

	FontFace        notLoaded;
	const FontFace* notLoadedFaces[] = {&notLoaded};
	EXPECT_FALSE(_shaper.Shape("A", notLoadedFaces, Size, layout));
}

// ============================================================================
// Ellipsis
// ============================================================================

TEST_F(TextShaperTest, EllipsizedLeavesWhatFits)
{
	const FontFace* faces[] = {_sans, _sc};
	TextLayout      whole = Shape("Hello world");
	TextLayout      layout;
	ASSERT_TRUE(_shaper.ShapeEllipsized("Hello world", faces, Size, whole._width, layout));

	ASSERT_EQ(layout._glyphs.Size(), whole._glyphs.Size());
	EXPECT_FLOAT_EQ(layout._width, whole._width);
}

TEST_F(TextShaperTest, EllipsizedCutsAndEndsWithTheEllipsis)
{
	const FontFace* faces[] = {_sans, _sc};
	TextLayout      whole = Shape("Hello world");
	const float     maxWidth = whole._width * 0.6f;

	TextLayout layout;
	ASSERT_TRUE(_shaper.ShapeEllipsized("Hello world", faces, Size, maxWidth, layout));

	EXPECT_LE(layout._width, maxWidth);
	ASSERT_LT(layout._glyphs.Size(), whole._glyphs.Size());

	// The same glyphs, then the ellipsis
	TextLayout ellipsis = Shape("\xE2\x80\xA6");
	ASSERT_EQ(ellipsis._glyphs.Size(), 1u);
	EXPECT_NE(ellipsis._glyphs[0]._glyphId, 0u);
	for (uint32_t glyphIndex = 0; glyphIndex + 1 < layout._glyphs.Size(); ++glyphIndex)
	{
		EXPECT_EQ(layout._glyphs[glyphIndex]._glyphId, whole._glyphs[glyphIndex]._glyphId);
	}
	EXPECT_EQ(layout._glyphs.Back()._glyphId, ellipsis._glyphs[0]._glyphId);
	EXPECT_EQ(layout._glyphs.Back()._cluster, layout._glyphs.Size() - 1); // one byte per character before it
}

// "Hello …" would not fit where "Hello…" does: the space before the cut goes
TEST_F(TextShaperTest, EllipsizedDropsTheSpacesBeforeTheCut)
{
	const FontFace* faces[] = {_sans, _sc};
	TextLayout      expected = Shape("Hello\xE2\x80\xA6");

	TextLayout layout;
	ASSERT_TRUE(_shaper.ShapeEllipsized("Hello world", faces, Size, expected._width + 1.0f, layout));

	ASSERT_EQ(layout._glyphs.Size(), expected._glyphs.Size());
	for (uint32_t glyphIndex = 0; glyphIndex < layout._glyphs.Size(); ++glyphIndex)
	{
		EXPECT_EQ(layout._glyphs[glyphIndex]._glyphId, expected._glyphs[glyphIndex]._glyphId);
	}
}

// An accented letter written as e + combining acute is cut whole
TEST_F(TextShaperTest, EllipsizedKeepsALetterWithItsMarks)
{
	const FontFace* faces[] = {_sans, _sc};
	const char*     text = "e\xCC\x81" "e\xCC\x81" "e\xCC\x81" "e\xCC\x81" "e\xCC\x81";
	TextLayout      whole = Shape(text);

	TextLayout layout;
	ASSERT_TRUE(_shaper.ShapeEllipsized(text, faces, Size, whole._width * 0.7f, layout));

	// Clusters 0, 3, 6...: never inside a letter
	for (const TextLayout::Glyph& glyph : layout._glyphs)
	{
		EXPECT_EQ(glyph._cluster % 3, 0u);
	}
}

TEST_F(TextShaperTest, EllipsizedLeavesTheEllipsisWhenNothingFits)
{
	const FontFace* faces[] = {_sans, _sc};
	TextLayout      layout;
	ASSERT_TRUE(_shaper.ShapeEllipsized("Hello world", faces, Size, 1.0f, layout));

	TextLayout ellipsis = Shape("\xE2\x80\xA6");
	ASSERT_EQ(layout._glyphs.Size(), 1u);
	ASSERT_EQ(ellipsis._glyphs.Size(), 1u);
	EXPECT_EQ(layout._glyphs[0]._glyphId, ellipsis._glyphs[0]._glyphId);
}

// ============================================================================
// Pixel art: Silkscreen, drawn on a grid of 8 pixels an em
// ============================================================================

TEST_F(TextShaperTest, FaceHasItsNamesAndCounts)
{
	EXPECT_EQ(_sans->GetFamilyName(), String("Noto Sans"));
	EXPECT_EQ(_sans->GetStyleName(), String("Regular"));
	EXPECT_GT(_sans->GetGlyphCount(), 1000u);
	EXPECT_GT(_sans->GetXHeight(), 400);
	EXPECT_LT(_sans->GetXHeight(), _sans->GetCapHeight());
}

TEST_F(TextShaperTest, OutlineFontHasNoPixelGrid)
{
	EXPECT_EQ(_sans->DetectPixelGrid(), 0u);
}

TEST(PixelArtFont, DetectsItsPixelGrid)
{
	std::vector<char> data = ReadFixture("Silkscreen-Regular.ttf");
	FontFace          face;
	ASSERT_TRUE(face.LoadFromMemory(data.data(), (uint32_t)data.size()));

	EXPECT_EQ(face.DetectPixelGrid(), 8u);
	EXPECT_EQ(face.GetFamilyName(), String("Silkscreen"));
}

TEST(PixelArtFont, AdvancesAreWholePixelsOfItsGrid)
{
	std::vector<char> data = ReadFixture("Silkscreen-Regular.ttf");
	FontFace          face;
	ASSERT_TRUE(face.LoadFromMemory(data.data(), (uint32_t)data.size()));
	EXPECT_EQ(face.GetUnitsPerEm(), 1000u); // 125 units a pixel

	const FontFace* faces[] = {&face};
	TextShaper      shaper;
	for (float size : {8.0f, 24.0f})
	{
		TextLayout layout;
		ASSERT_TRUE(shaper.Shape("Hello, World!", faces, size, layout));
		for (const TextLayout::Glyph& glyph : layout._glyphs)
		{
			const float gridPixels = glyph._position.GetX() * 8.0f / size;
			EXPECT_FLOAT_EQ(gridPixels, std::round(gridPixels)) << size;
		}
	}
}
