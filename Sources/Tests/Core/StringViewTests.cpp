#include <gtest/gtest.h>
#include <HodEngine/Core/StringView.hpp>

#include <string_view>
#include <unordered_map>

class StringView : public ::testing::Test
{
protected:
	void SetUp() override {}
	void TearDown() override {}
};

// ============================================================================
// Construction
// ============================================================================

TEST_F(StringView, DefaultConstructor)
{
	hod::StringView view;
	EXPECT_EQ(view.Size(), 0);
	EXPECT_TRUE(view.Empty());
	EXPECT_EQ(view.Data(), nullptr);
}

TEST_F(StringView, ConstructorFromCStr)
{
	hod::StringView view("hello");
	EXPECT_EQ(view.Size(), 5);
	EXPECT_FALSE(view.Empty());
	EXPECT_EQ(view, "hello");
}

TEST_F(StringView, ConstructorFromCStrAndSize)
{
	hod::StringView view("hello world", 5);
	EXPECT_EQ(view.Size(), 5);
	EXPECT_EQ(view, "hello");
}

TEST_F(StringView, ConstructorFromString)
{
	hod::String str("hello");
	hod::StringView view(str);
	EXPECT_EQ(view.Size(), str.Size());
	EXPECT_EQ(view, "hello");
}

TEST_F(StringView, ConstructorFromStdStringView)
{
	std::string_view stdView("hello");
	hod::StringView  view(stdView);
	EXPECT_EQ(view.Size(), 5);
	EXPECT_EQ(view, "hello");
}

TEST_F(StringView, ConvertsToStdStringView)
{
	hod::StringView   view("hello");
	std::string_view  stdView = view;
	EXPECT_EQ(stdView, "hello");
}

TEST_F(StringView, CopyIsShallow)
{
	hod::StringView view("hello");
	hod::StringView copy(view);
	EXPECT_EQ(copy.Data(), view.Data());
	EXPECT_EQ(copy.Size(), view.Size());
}

TEST_F(StringView, IsConstexprConstructible)
{
	constexpr hod::StringView view("hello");
	static_assert(view.Size() == 5);
	static_assert(view == hod::StringView("hello"));
	EXPECT_EQ(view.Size(), 5);
}

// ============================================================================
// Accessors
// ============================================================================

TEST_F(StringView, IndexOperator)
{
	hod::StringView view("hello");
	EXPECT_EQ(view[0], 'h');
	EXPECT_EQ(view[4], 'o');
}

TEST_F(StringView, At)
{
	hod::StringView view("hello");
	EXPECT_EQ(view.At(1), 'e');
}

TEST_F(StringView, FrontBack)
{
	hod::StringView view("hello");
	EXPECT_EQ(view.Front(), 'h');
	EXPECT_EQ(view.Back(), 'o');
}

TEST_F(StringView, RangeBasedFor)
{
	hod::StringView view("abc");
	std::string     result;
	for (char c : view)
	{
		result += c;
	}
	EXPECT_EQ(result, "abc");
}

// ============================================================================
// Mutators (view rebinding, not the underlying data)
// ============================================================================

TEST_F(StringView, RemovePrefix)
{
	hod::StringView view("hello world");
	view.RemovePrefix(6);
	EXPECT_EQ(view, "world");
}

TEST_F(StringView, RemoveSuffix)
{
	hod::StringView view("hello world");
	view.RemoveSuffix(6);
	EXPECT_EQ(view, "hello");
}

TEST_F(StringView, Swap)
{
	hod::StringView a("hello");
	hod::StringView b("world");
	a.Swap(b);
	EXPECT_EQ(a, "world");
	EXPECT_EQ(b, "hello");
}

TEST_F(StringView, SubStr)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.SubStr(6), "world");
	EXPECT_EQ(view.SubStr(0, 5), "hello");
}

TEST_F(StringView, SubStrClampsCount)
{
	hod::StringView view("hello");
	EXPECT_EQ(view.SubStr(2, 100), "llo");
}

TEST_F(StringView, SubStrIsNotNullTerminatedButKeepsSize)
{
	hod::StringView view("hello world");
	hod::StringView sub = view.SubStr(0, 5);
	EXPECT_EQ(sub.Size(), 5);
	EXPECT_EQ(sub.Data(), view.Data());
}

// ============================================================================
// Find
// ============================================================================

TEST_F(StringView, FindChar)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.Find('o'), 4);
	EXPECT_EQ(view.Find('o', 5), 7);
	EXPECT_EQ(view.Find('z'), hod::StringView::Npos);
}

TEST_F(StringView, FindStringView)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.Find(hod::StringView("world")), 6);
	EXPECT_EQ(view.Find(hod::StringView("xyz")), hod::StringView::Npos);
}

TEST_F(StringView, FindCStrImplicitConversion)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.Find("world"), 6);
}

TEST_F(StringView, FindREmptyMatchesEnd)
{
	hod::StringView view("hello");
	EXPECT_EQ(view.FindR(""), view.Size());
}

TEST_F(StringView, FindRChar)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.FindR('o'), 7);
}

TEST_F(StringView, FindRStringView)
{
	hod::StringView view("hello hello");
	EXPECT_EQ(view.FindR(hod::StringView("hello")), 6);
}

TEST_F(StringView, FindFirstOf)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.FindFirstOf("ow"), 4);
}

TEST_F(StringView, FindLastOf)
{
	hod::StringView view("hello world");
	EXPECT_EQ(view.FindLastOf("ow"), 7);
}

// ============================================================================
// Compare / StartsWith / EndsWith / Contains
// ============================================================================

TEST_F(StringView, CompareEqual)
{
	EXPECT_EQ(hod::StringView("abc").Compare("abc"), 0);
}

TEST_F(StringView, CompareLess)
{
	EXPECT_LT(hod::StringView("abc").Compare("abd"), 0);
}

TEST_F(StringView, CompareShorterIsLess)
{
	EXPECT_LT(hod::StringView("ab").Compare("abc"), 0);
}

TEST_F(StringView, StartsWith)
{
	hod::StringView view("hello world");
	EXPECT_TRUE(view.StartsWith('h'));
	EXPECT_TRUE(view.StartsWith("hello"));
	EXPECT_FALSE(view.StartsWith("world"));
}

TEST_F(StringView, EndsWith)
{
	hod::StringView view("hello world");
	EXPECT_TRUE(view.EndsWith('d'));
	EXPECT_TRUE(view.EndsWith("world"));
	EXPECT_FALSE(view.EndsWith("hello"));
}

TEST_F(StringView, Contains)
{
	hod::StringView view("hello world");
	EXPECT_TRUE(view.Contains('w'));
	EXPECT_TRUE(view.Contains("lo wo"));
	EXPECT_FALSE(view.Contains("xyz"));
}

// ============================================================================
// Comparison operators (also exercise implicit conversion from const char*, String, std::string_view)
// ============================================================================

TEST_F(StringView, EqualityWithCStr)
{
	hod::StringView view("hello");
	EXPECT_TRUE(view == "hello");
	EXPECT_TRUE("hello" == view);
	EXPECT_FALSE(view != "hello");
}

TEST_F(StringView, EqualityWithString)
{
	hod::String     str("hello");
	hod::StringView view("hello");
	EXPECT_TRUE(view == str);
}

TEST_F(StringView, EqualityWithStdStringView)
{
	hod::StringView   view("hello");
	std::string_view stdView("hello");
	EXPECT_TRUE(view == stdView);
}

TEST_F(StringView, Ordering)
{
	EXPECT_LT(hod::StringView("abc"), hod::StringView("abd"));
	EXPECT_LE(hod::StringView("abc"), hod::StringView("abc"));
	EXPECT_GT(hod::StringView("abd"), hod::StringView("abc"));
	EXPECT_GE(hod::StringView("abc"), hod::StringView("abc"));
}

// ============================================================================
// Hash / fmt / literal
// ============================================================================

TEST_F(StringView, Hashable)
{
	std::unordered_map<hod::StringView, int32_t> map;
	map[hod::StringView("hello")] = 42;
	EXPECT_EQ(map[hod::StringView("hello")], 42);
}

TEST_F(StringView, FormatWithFmt)
{
	hod::StringView view("hello");
	EXPECT_EQ(fmt::format("{}", view), "hello");
}

TEST_F(StringView, UserDefinedLiteral)
{
	using namespace hod::String_Literals;
	hod::StringView view = "hello"_sv;
	EXPECT_EQ(view, "hello");
}
