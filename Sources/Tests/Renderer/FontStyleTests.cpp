#include <gtest/gtest.h>

#include <HodEngine/Renderer/Resource/FontResource.hpp>

using namespace hod;

namespace
{
	using Weight = FontResource::Weight;

	FontResource::Style MakeStyle(uint64_t id, Weight weight, bool italic)
	{
		FontResource::Style style;
		style._face = UID(id, 0);
		style._weight = weight;
		style._italic = italic;
		return style;
	}
}

TEST(FontStyle, WithoutStylesTheFontItself)
{
	FontResource font;
	EXPECT_FALSE(font.FindStyle(Weight::Bold, true).IsValid());
}

TEST(FontStyle, TheExactOne)
{
	FontResource font;
	font.SetStyles({MakeStyle(4, Weight::Regular, false), MakeStyle(1, Weight::Bold, false), MakeStyle(2, Weight::Regular, true), MakeStyle(3, Weight::Bold, true)});

	EXPECT_EQ(font.FindStyle(Weight::Regular, false), UID(4, 0));
	EXPECT_EQ(font.FindStyle(Weight::Bold, false), UID(1, 0));
	EXPECT_EQ(font.FindStyle(Weight::Regular, true), UID(2, 0));
	EXPECT_EQ(font.FindStyle(Weight::Bold, true), UID(3, 0));
}

TEST(FontStyle, TheItalicBeforeTheWeight)
{
	FontResource font;
	font.SetStyles({MakeStyle(1, Weight::Bold, false), MakeStyle(2, Weight::Light, true)});

	EXPECT_EQ(font.FindStyle(Weight::Bold, true), UID(2, 0));
}

TEST(FontStyle, UprightWhenTheFamilyHasNoItalic)
{
	FontResource font;
	font.SetStyles({MakeStyle(1, Weight::Bold, false), MakeStyle(2, Weight::Regular, false)});

	EXPECT_EQ(font.FindStyle(Weight::Black, true), UID(1, 0));
	EXPECT_EQ(font.FindStyle(Weight::Regular, true), UID(2, 0));
}

TEST(FontStyle, TheWeightAsCssMatchesIt)
{
	FontResource font;
	font.SetStyles({MakeStyle(4, Weight::Regular, false), MakeStyle(1, Weight::Light, false), MakeStyle(2, Weight::SemiBold, false),
	                MakeStyle(3, Weight::Black, false)});

	// Bolder than 500: the bolder ones first
	EXPECT_EQ(font.FindStyle(Weight::Bold, false), UID(3, 0));
	EXPECT_EQ(font.FindStyle(Weight::SemiBold, false), UID(2, 0));
	// Lighter than 400: the lighter ones first, then the bolder
	EXPECT_EQ(font.FindStyle(Weight::ExtraLight, false), UID(1, 0));
	EXPECT_EQ(font.FindStyle(Weight::Thin, false), UID(1, 0));
	// 500: up to 500, then the lighter
	EXPECT_EQ(font.FindStyle(Weight::Medium, false), UID(4, 0));
}

TEST(FontStyle, OneFontForSeveralStyles)
{
	FontResource font;
	font.SetStyles({MakeStyle(1, Weight::Regular, false), MakeStyle(2, Weight::Bold, false), MakeStyle(1, Weight::Medium, false), MakeStyle(2, Weight::Black, false)});

	EXPECT_EQ(font.FindStyle(Weight::Medium, false), UID(1, 0));
	EXPECT_EQ(font.FindStyle(Weight::ExtraBold, false), UID(2, 0));
	EXPECT_EQ(font.FindStyle(Weight::Black, false), UID(2, 0));
}

TEST(FontStyle, NoneSetIsLeftOut)
{
	FontResource font;
	font.SetStyles({MakeStyle(0, Weight::Bold, false)});

	EXPECT_FALSE(font.FindStyle(Weight::Bold, false).IsValid());
}
