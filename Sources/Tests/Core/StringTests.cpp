#include <gtest/gtest.h>
#include <HodEngine/Core/String.hpp>

#include <string_view>

// ============================================================================
// Base Fixture pour tous les tests String
// ============================================================================

class String : public ::testing::Test
{
protected:
	void SetUp() override {}
	void TearDown() override {}
};

// ============================================================================
// Construction & Destruction Tests
// ============================================================================

TEST_F(String, DefaultConstructor)
{
	hod::String str;
	EXPECT_EQ(str.Size(), 0);
	EXPECT_EQ(str.Length(), 0);
	EXPECT_TRUE(str.Empty());
	EXPECT_STREQ(str.CStr(), "");
}

TEST_F(String, ConstructorFromCStr)
{
	hod::String str("hello");
	EXPECT_EQ(str.Size(), 5);
	EXPECT_FALSE(str.Empty());
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, ConstructorFromCStrWithLength)
{
	hod::String str("hello world", 5);
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, ConstructorFromCountAndChar)
{
	hod::String str(5, 'a');
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "aaaaa");
}

TEST_F(String, ConstructorWithCapacity)
{
	hod::String str(32u);
	EXPECT_EQ(str.Size(), 0);
	EXPECT_GE(str.Capacity(), 32u);
}

TEST_F(String, CopyConstructor)
{
	hod::String original("hello");
	hod::String copy(original);

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_STREQ(copy.CStr(), original.CStr());

	// Verify deep copy
	copy[0] = 'H';
	EXPECT_STRNE(copy.CStr(), original.CStr());
}

TEST_F(String, MoveConstructor)
{
	hod::String original("hello");
	const uint32_t originalSize = original.Size();

	hod::String moved(std::move(original));

	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_STREQ(moved.CStr(), "hello");
	EXPECT_EQ(original.Size(), 0);
	EXPECT_TRUE(original.Empty());
}

TEST_F(String, ConstructorFromStringView)
{
	std::string_view view("hello world");
	hod::String str(view);
	EXPECT_EQ(str.Size(), 11);
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, ConstructorFromStringViewWithRange)
{
	std::string_view view("hello world");
	hod::String str(view, 6, 5);
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, CopyConstructorSubstring)
{
	hod::String original("hello world");
	hod::String sub(original, 6, 5);
	EXPECT_EQ(sub.Size(), 5);
	EXPECT_STREQ(sub.CStr(), "world");
}

TEST_F(String, CopyConstructorSubstringToEnd)
{
	hod::String original("hello world");
	hod::String sub(original, 6);
	EXPECT_EQ(sub.Size(), 5);
	EXPECT_STREQ(sub.CStr(), "world");
}

// ============================================================================
// SSO (Small String Optimization) Tests
// ============================================================================

TEST_F(String, SmallStringOptimization)
{
	// Strings < 16 chars should use inline buffer (no heap allocation)
	hod::String small("short");
	EXPECT_EQ(small.Size(), 5);
	EXPECT_STREQ(small.CStr(), "short");
}

TEST_F(String, LargeStringExceedsSSO)
{
	// Strings >= 16 chars use heap allocation
	hod::String large("this is a long string that exceeds SSO");
	EXPECT_EQ(large.Size(), 38);
	EXPECT_STREQ(large.CStr(), "this is a long string that exceeds SSO");
}

// ============================================================================
// Assignment Operators Tests
// ============================================================================

TEST_F(String, AssignFromChar)
{
	hod::String str("hello");
	str = 'X';
	EXPECT_EQ(str.Size(), 1);
	EXPECT_STREQ(str.CStr(), "X");
}

TEST_F(String, AssignFromCStr)
{
	hod::String str("hello");
	str = "world";
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, AssignFromString)
{
	hod::String str("hello");
	hod::String other("world");
	str = other;
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, SelfAssignment)
{
	hod::String str("hello");

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
	str = str;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, MoveAssignment)
{
	hod::String str("hello");
	hod::String other("world");

	str = std::move(other);
	EXPECT_STREQ(str.CStr(), "world");
	EXPECT_TRUE(other.Empty());
}

TEST_F(String, AssignFromStringView)
{
	hod::String str("hello");
	str = std::string_view("world");
	EXPECT_STREQ(str.CStr(), "world");
}

// ============================================================================
// Concatenation Tests
// ============================================================================

TEST_F(String, PlusEqualsChar)
{
	hod::String str("hello");
	str += '!';
	EXPECT_STREQ(str.CStr(), "hello!");
	EXPECT_EQ(str.Size(), 6);
}

TEST_F(String, PlusEqualsCStr)
{
	hod::String str("hello");
	str += " world";
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, PlusEqualsString)
{
	hod::String str("hello");
	hod::String other(" world");
	str += other;
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, PlusEqualsStringView)
{
	hod::String str("hello");
	str += std::string_view(" world");
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, PlusChar)
{
	hod::String str("hello");
	hod::String result = str + '!';
	EXPECT_STREQ(result.CStr(), "hello!");
	EXPECT_STREQ(str.CStr(), "hello"); // Original unchanged
}

TEST_F(String, PlusCStr)
{
	hod::String str("hello");
	hod::String result = str + " world";
	EXPECT_STREQ(result.CStr(), "hello world");
}

TEST_F(String, PlusString)
{
	hod::String str("hello");
	hod::String other(" world");
	hod::String result = str + other;
	EXPECT_STREQ(result.CStr(), "hello world");
}

TEST_F(String, PlusCStrOnLeft)
{
	hod::String str("world");
	hod::String result = "hello " + str;
	EXPECT_STREQ(result.CStr(), "hello world");
}

// ============================================================================
// Comparison Tests
// ============================================================================

TEST_F(String, EqualityString)
{
	hod::String str1("hello");
	hod::String str2("hello");
	hod::String str3("world");

	EXPECT_TRUE(str1 == str2);
	EXPECT_FALSE(str1 == str3);
}

TEST_F(String, EqualityCStr)
{
	hod::String str("hello");
	EXPECT_TRUE(str == "hello");
	EXPECT_FALSE(str == "world");
}

TEST_F(String, InequalityString)
{
	hod::String str1("hello");
	hod::String str2("world");
	EXPECT_TRUE(str1 != str2);
	EXPECT_FALSE(str1 != str1);
}

TEST_F(String, LessThan)
{
	hod::String str1("abc");
	hod::String str2("def");
	EXPECT_TRUE(str1 < str2);
	EXPECT_FALSE(str2 < str1);
}

TEST_F(String, LessThanOrEqual)
{
	hod::String str1("abc");
	hod::String str2("abc");
	hod::String str3("def");
	EXPECT_TRUE(str1 <= str2);
	EXPECT_TRUE(str1 <= str3);
	EXPECT_FALSE(str3 <= str1);
}

TEST_F(String, GreaterThan)
{
	hod::String str1("def");
	hod::String str2("abc");
	EXPECT_TRUE(str1 > str2);
	EXPECT_FALSE(str2 > str1);
}

TEST_F(String, GreaterThanOrEqual)
{
	hod::String str1("def");
	hod::String str2("def");
	hod::String str3("abc");
	EXPECT_TRUE(str1 >= str2);
	EXPECT_TRUE(str1 >= str3);
	EXPECT_FALSE(str3 >= str1);
}

TEST_F(String, CStrComparisonLeftSide)
{
	hod::String str("hello");
	EXPECT_TRUE("hello" == str);
	EXPECT_FALSE("world" == str);
	EXPECT_TRUE("world" != str);
}

// ============================================================================
// Element Access Tests
// ============================================================================

TEST_F(String, OperatorBracket)
{
	hod::String str("hello");
	EXPECT_EQ(str[0], 'h');
	EXPECT_EQ(str[4], 'o');

	str[0] = 'H';
	EXPECT_EQ(str[0], 'H');
}

TEST_F(String, AtMethod)
{
	hod::String str("hello");
	EXPECT_EQ(str.At(0), 'h');
	EXPECT_EQ(str.At(4), 'o');

	str.At(0) = 'H';
	EXPECT_EQ(str.At(0), 'H');
}

TEST_F(String, FrontAndBack)
{
	hod::String str("hello");
	EXPECT_EQ(str.Front(), 'h');
	EXPECT_EQ(str.Back(), 'o');

	str.Front() = 'H';
	str.Back() = 'O';
	EXPECT_EQ(str.Front(), 'H');
	EXPECT_EQ(str.Back(), 'O');
}

TEST_F(String, DataAccess)
{
	hod::String str("hello");
	char* data = str.Data();
	EXPECT_NE(data, nullptr);
	EXPECT_EQ(data[0], 'h');
}

TEST_F(String, CStrIsNullTerminated)
{
	hod::String str("hello");
	const char* cstr = str.CStr();
	EXPECT_EQ(cstr[str.Size()], '\0');
}

TEST_F(String, StringViewConversion)
{
	hod::String str("hello");
	std::string_view view = str;
	EXPECT_EQ(view.size(), 5);
	EXPECT_EQ(view, "hello");
}

// ============================================================================
// Iterator Tests
// ============================================================================

TEST_F(String, RangeBasedFor)
{
	hod::String str("hello");
	std::string collected;
	for (char c : str)
	{
		collected += c;
	}
	EXPECT_EQ(collected, "hello");
}

TEST_F(String, BeginEnd)
{
	hod::String str("hello");
	int count = 0;
	for (char* it = str.begin(); it != str.end(); ++it)
	{
		++count;
	}
	EXPECT_EQ(count, 5);
}

TEST_F(String, ConstBeginEnd)
{
	const hod::String str("hello");
	int count = 0;
	for (const char* it = str.begin(); it != str.end(); ++it)
	{
		++count;
	}
	EXPECT_EQ(count, 5);
}

// ============================================================================
// Capacity Tests
// ============================================================================

TEST_F(String, Reserve)
{
	hod::String str;
	str.Reserve(100);
	EXPECT_GE(str.Capacity(), 100u);
	EXPECT_EQ(str.Size(), 0);
}

TEST_F(String, ReserveDoesNotChangeSize)
{
	hod::String str("hello");
	str.Reserve(100);
	EXPECT_GE(str.Capacity(), 100u);
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, ShrinkToFit)
{
	hod::String str;
	str.Reserve(100);
	str.Append("hi");
	str.ShrinkToFit();
	EXPECT_EQ(str.Capacity(), str.Size());
	EXPECT_STREQ(str.CStr(), "hi");
}

TEST_F(String, Clear)
{
	hod::String str("hello");
	uint32_t oldCapacity = str.Capacity();
	str.Clear();
	EXPECT_EQ(str.Size(), 0);
	EXPECT_TRUE(str.Empty());
	EXPECT_EQ(str.Capacity(), oldCapacity); // Capacity unchanged
}

TEST_F(String, ResizeGrow)
{
	hod::String str("hello");
	str.Resize(10);
	EXPECT_EQ(str.Size(), 10);
	EXPECT_EQ(str[0], 'h');
	EXPECT_EQ(str[4], 'o');
}

TEST_F(String, ResizeShrink)
{
	hod::String str("hello world");
	str.Resize(5);
	EXPECT_EQ(str.Size(), 5);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, ResizeWithChar)
{
	hod::String str("hello");
	str.Resize(8, '!');
	EXPECT_EQ(str.Size(), 8);
	EXPECT_EQ(str[5], '!');
	EXPECT_EQ(str[6], '!');
	EXPECT_EQ(str[7], '!');
}

// ============================================================================
// PushBack / PopBack Tests
// ============================================================================

TEST_F(String, PushBack)
{
	hod::String str;
	str.PushBack('h');
	str.PushBack('i');
	EXPECT_EQ(str.Size(), 2);
	EXPECT_STREQ(str.CStr(), "hi");
}

TEST_F(String, PopBack)
{
	hod::String str("hello");
	str.PopBack();
	EXPECT_EQ(str.Size(), 4);
	EXPECT_STREQ(str.CStr(), "hell");
}

// ============================================================================
// Assign Tests
// ============================================================================

TEST_F(String, AssignCountChar)
{
	hod::String str("hello");
	str.Assign(3, 'x');
	EXPECT_STREQ(str.CStr(), "xxx");
}

TEST_F(String, AssignCStr)
{
	hod::String str("hello");
	str.Assign("world");
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, AssignCStrWithSize)
{
	hod::String str("hello");
	str.Assign("world!!!", 5);
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, AssignString)
{
	hod::String str("hello");
	hod::String other("world");
	str.Assign(other);
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, AssignStringSubstring)
{
	hod::String str("hello");
	hod::String other("hello world");
	str.Assign(other, 6, 5);
	EXPECT_STREQ(str.CStr(), "world");
}

TEST_F(String, AssignMove)
{
	hod::String str("hello");
	hod::String other("world");
	str.Assign(std::move(other));
	EXPECT_STREQ(str.CStr(), "world");
	EXPECT_TRUE(other.Empty());
}

// ============================================================================
// Append Tests
// ============================================================================

TEST_F(String, AppendCountChar)
{
	hod::String str("hello");
	str.Append(3, '!');
	EXPECT_STREQ(str.CStr(), "hello!!!");
}

TEST_F(String, AppendCStr)
{
	hod::String str("hello");
	str.Append(" world");
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, AppendString)
{
	hod::String str("hello");
	hod::String other(" world");
	str.Append(other);
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, AppendStringSubstring)
{
	hod::String str("hello");
	hod::String other(" world!!!");
	str.Append(other, 0, 6);
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, AppendStringView)
{
	hod::String str("hello");
	str.Append(std::string_view(" world"));
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, AppendStringViewSubstring)
{
	hod::String str("ab");
	str.Append(std::string_view("xyz"), 0, 3);
	EXPECT_EQ(str.Size(), 5u);
	EXPECT_STREQ(str.CStr(), "abxyz");
}

// ============================================================================
// Insert Tests
// ============================================================================

TEST_F(String, InsertCountChar)
{
	hod::String str("hllo");
	str.Insert(1, 1, 'e');
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, InsertCStr)
{
	hod::String str("hllo");
	str.Insert(1, "e");
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, InsertCStrWithCount)
{
	hod::String str("hllo");
	str.Insert(1, "eXX", 1);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, InsertString)
{
	hod::String str("hllo");
	hod::String ins("e");
	str.Insert(1, ins);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, InsertAtBeginning)
{
	hod::String str("world");
	str.Insert(0, "hello ");
	EXPECT_STREQ(str.CStr(), "hello world");
}

TEST_F(String, InsertAtEnd)
{
	hod::String str("hello");
	str.Insert(5, " world");
	EXPECT_STREQ(str.CStr(), "hello world");
}

// ============================================================================
// Replace Tests
// ============================================================================

TEST_F(String, ReplaceWithCountChar)
{
	hod::String str("hello world");
	str.Replace(6, 5, 3, 'X');
	EXPECT_STREQ(str.CStr(), "hello XXX");
}

TEST_F(String, ReplaceWithCStr)
{
	hod::String str("hello world");
	str.Replace(6, 5, "there");
	EXPECT_STREQ(str.CStr(), "hello there");
}

TEST_F(String, ReplaceWithString)
{
	hod::String str("hello world");
	hod::String rep("there");
	str.Replace(6, 5, rep);
	EXPECT_STREQ(str.CStr(), "hello there");
}

TEST_F(String, ReplaceShorter)
{
	hod::String str("hello world");
	str.Replace(5, 6, "!");
	EXPECT_STREQ(str.CStr(), "hello!");
}

TEST_F(String, ReplaceLonger)
{
	hod::String str("hi");
	str.Replace(0, 2, "hello world");
	EXPECT_STREQ(str.CStr(), "hello world");
}

// ============================================================================
// Erase Tests
// ============================================================================

TEST_F(String, EraseFromPosition)
{
	hod::String str("hello world");
	str.Erase(5);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, EraseWithLength)
{
	hod::String str("hello world");
	str.Erase(5, 6);
	EXPECT_STREQ(str.CStr(), "hello");
}

TEST_F(String, EraseMiddle)
{
	hod::String str("hello world");
	str.Erase(5, 1);
	EXPECT_STREQ(str.CStr(), "helloworld");
}

// ============================================================================
// SubStr Tests
// ============================================================================

TEST_F(String, SubStrFromPosition)
{
	hod::String str("hello world");
	hod::String sub = str.SubStr(6);
	EXPECT_STREQ(sub.CStr(), "world");
}

TEST_F(String, SubStrWithLength)
{
	hod::String str("hello world");
	hod::String sub = str.SubStr(0, 5);
	EXPECT_STREQ(sub.CStr(), "hello");
}

TEST_F(String, SubStrMiddle)
{
	hod::String str("hello world");
	hod::String sub = str.SubStr(6, 3);
	EXPECT_STREQ(sub.CStr(), "wor");
}

// ============================================================================
// Swap Tests
// ============================================================================

TEST_F(String, Swap)
{
	hod::String str1("hello");
	hod::String str2("world");
	str1.Swap(str2);
	EXPECT_STREQ(str1.CStr(), "world");
	EXPECT_STREQ(str2.CStr(), "hello");
}

// ============================================================================
// Find Tests
// ============================================================================

TEST_F(String, FindChar)
{
	hod::String str("hello world");
	EXPECT_EQ(str.Find('o'), 4u);
	EXPECT_EQ(str.Find('o', 5), 7u);
	EXPECT_EQ(str.Find('z'), hod::String::Npos);
}

TEST_F(String, FindCStr)
{
	hod::String str("hello world");
	EXPECT_EQ(str.Find("world"), 6u);
	EXPECT_EQ(str.Find("xyz"), hod::String::Npos);
}

TEST_F(String, FindString)
{
	hod::String str("hello world");
	hod::String search("world");
	EXPECT_EQ(str.Find(search), 6u);
}

TEST_F(String, FindStringView)
{
	hod::String str("hello world");
	EXPECT_EQ(str.Find(std::string_view("world")), 6u);
}

TEST_F(String, FindR_Char)
{
	hod::String str("hello world");
	EXPECT_EQ(str.FindR('l'), 9u);
}

TEST_F(String, FindR_CStr)
{
	hod::String str("hello hello");
	EXPECT_EQ(str.FindR("hello"), 6u);
}

TEST_F(String, FindFirstOf)
{
	hod::String str("hello world");
	EXPECT_EQ(str.FindFirstOf("aeiou"), 1u); // 'e' at index 1
}

TEST_F(String, FindLastOf)
{
	hod::String str("hello world");
	EXPECT_EQ(str.FindLastOf("aeiou"), 7u); // 'o' at index 7
}

// ============================================================================
// Compare Tests
// ============================================================================

TEST_F(String, CompareCStr)
{
	hod::String str("hello");
	EXPECT_EQ(str.Compare("hello"), 0);
	EXPECT_LT(str.Compare("world"), 0);
	EXPECT_GT(str.Compare("abc"), 0);
}

TEST_F(String, CompareString)
{
	hod::String str("hello");
	hod::String other("hello");
	EXPECT_EQ(str.Compare(other), 0);
}

TEST_F(String, CompareSubstring)
{
	hod::String str("hello world");
	EXPECT_EQ(str.Compare(0, 5, "hello"), 0);
	EXPECT_EQ(str.Compare(6, 5, "world"), 0);
}

TEST_F(String, CompareStringViewNonNullTerminated)
{
	char buf[] = { 'h', 'e', 'l', 'l', 'o', 'X' };
	std::string_view view(buf, 5);

	hod::String str("hello");
	EXPECT_EQ(str.Compare(view), 0);
}

// ============================================================================
// StartsWith / EndsWith / Contains Tests
// ============================================================================

TEST_F(String, StartsWithChar)
{
	hod::String str("hello");
	EXPECT_TRUE(str.StartsWith('h'));
	EXPECT_FALSE(str.StartsWith('e'));
}

TEST_F(String, StartsWithCStr)
{
	hod::String str("hello world");
	EXPECT_TRUE(str.StartsWith("hello"));
	EXPECT_FALSE(str.StartsWith("world"));
}

TEST_F(String, StartsWithString)
{
	hod::String str("hello world");
	hod::String prefix("hello");
	EXPECT_TRUE(str.StartsWith(prefix));
	EXPECT_FALSE(str.StartsWith(hod::String("world")));
}

TEST_F(String, StartsWithStringView)
{
	hod::String str("hello world");
	EXPECT_TRUE(str.StartsWith(std::string_view("hello")));
	EXPECT_FALSE(str.StartsWith(std::string_view("world")));
}

TEST_F(String, EndsWithChar)
{
	hod::String str("hello");
	EXPECT_TRUE(str.EndsWith('o'));
	EXPECT_FALSE(str.EndsWith('h'));
}

TEST_F(String, EndsWithCStr)
{
	hod::String str("hello world");
	EXPECT_TRUE(str.EndsWith("world"));
	EXPECT_FALSE(str.EndsWith("hello"));
}

TEST_F(String, EndsWithString)
{
	hod::String str("hello world");
	EXPECT_TRUE(str.EndsWith(hod::String("world")));
	EXPECT_FALSE(str.EndsWith(hod::String("hello")));
}

TEST_F(String, ContainsChar)
{
	hod::String str("hello");
	EXPECT_TRUE(str.Contains('e'));
	EXPECT_FALSE(str.Contains('z'));
}

TEST_F(String, ContainsCStr)
{
	hod::String str("hello world");
	EXPECT_TRUE(str.Contains("world"));
	EXPECT_FALSE(str.Contains("xyz"));
}

TEST_F(String, ContainsString)
{
	hod::String str("hello world");
	EXPECT_TRUE(str.Contains(hod::String("world")));
	EXPECT_FALSE(str.Contains(hod::String("xyz")));
}

TEST_F(String, ContainsOverlappingPattern)
{
	hod::String str("aaab");
	EXPECT_TRUE(str.Contains("aab"));

	hod::String str2("aababc");
	EXPECT_TRUE(str2.Contains("ababc"));
}

// ============================================================================
// Format Tests
// ============================================================================

TEST_F(String, FormatStatic)
{
	hod::String str = hod::String::Format("hello %s, you are %d years old", "world", 42);
	EXPECT_STREQ(str.CStr(), "hello world, you are 42 years old");
}

TEST_F(String, FormatInPlace)
{
	hod::String str;
	hod::String::Format(str, "value: %d", 99);
	EXPECT_STREQ(str.CStr(), "value: 99");
}

// ============================================================================
// String Literal Tests
// ============================================================================

TEST_F(String, StringLiteral)
{
	using namespace hod::String_Literals;
	hod::String str = "hello"_s;
	EXPECT_STREQ(str.CStr(), "hello");
	EXPECT_EQ(str.Size(), 5);
}

// ============================================================================
// Edge Cases Tests
// ============================================================================

TEST_F(String, EmptyStringOperations)
{
	hod::String str;
	EXPECT_TRUE(str.Empty());
	EXPECT_EQ(str.Size(), 0);
	EXPECT_STREQ(str.CStr(), "");
	str.Clear();
	EXPECT_TRUE(str.Empty());
}

TEST_F(String, LargeString)
{
	hod::String str;
	for (int i = 0; i < 1000; ++i)
	{
		str.PushBack('a');
	}
	EXPECT_EQ(str.Size(), 1000);
	EXPECT_EQ(str.Capacity(), str.Capacity()); // just verifying no crash
	for (uint32_t i = 0; i < str.Size(); ++i)
	{
		EXPECT_EQ(str[i], 'a');
	}
}

TEST_F(String, NullptrCStr)
{
	hod::String str(nullptr);
	EXPECT_EQ(str.Size(), 0);
	EXPECT_TRUE(str.Empty());
}

TEST_F(String, ConcatenationChain)
{
	hod::String str("a");
	str += "b";
	str += "c";
	str += "d";
	EXPECT_STREQ(str.CStr(), "abcd");
	EXPECT_EQ(str.Size(), 4);
}

TEST_F(String, SizeEqualsLength)
{
	hod::String str("hello");
	EXPECT_EQ(str.Size(), str.Length());
}
