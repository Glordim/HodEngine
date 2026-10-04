#include <gtest/gtest.h>
#include <HodEngine/Core/StaticArray.hpp>

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/String.hpp>

class StaticArray : public ::testing::Test
{
protected:
	void SetUp() override {}
	void TearDown() override {}
};

namespace
{
	uint32_t Sum(hod::ArrayView<int> view)
	{
		uint32_t sum = 0;
		for (int value : view)
		{
			sum += value;
		}
		return sum;
	}
}

// ============================================================================
// Construction
// ============================================================================

TEST_F(StaticArray, AggregateInitialization)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	EXPECT_EQ(array[0], 1);
	EXPECT_EQ(array[1], 2);
	EXPECT_EQ(array[2], 3);
}

TEST_F(StaticArray, MissingInitializersAreValueInitialized)
{
	hod::StaticArray<int, 4> array = {1};
	EXPECT_EQ(array[0], 1);
	EXPECT_EQ(array[1], 0);
	EXPECT_EQ(array[3], 0);

	hod::StaticArray<int*, 2> pointers = {nullptr};
	EXPECT_EQ(pointers[1], nullptr);

	hod::StaticArray<int, 2> zeroed = {};
	EXPECT_EQ(zeroed[0], 0);
	EXPECT_EQ(zeroed[1], 0);
}

TEST_F(StaticArray, NonTrivialType)
{
	hod::StaticArray<hod::String, 2> array = {hod::String("hello"), hod::String("world")};
	EXPECT_EQ(array[0], "hello");
	EXPECT_EQ(array[1], "world");

	hod::StaticArray<hod::String, 2> defaulted;
	EXPECT_EQ(defaulted[0].Size(), 0);
}

TEST_F(StaticArray, DeductionGuide)
{
	hod::StaticArray array = {1, 2, 3};
	static_assert(std::is_same_v<decltype(array), hod::StaticArray<int, 3>>);
	EXPECT_EQ(array.Size(), 3);
}

TEST_F(StaticArray, Copy)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	hod::StaticArray<int, 3> copy = array;
	copy[0] = 9;
	EXPECT_EQ(array[0], 1);
	EXPECT_EQ(copy[0], 9);

	array = copy;
	EXPECT_EQ(array[0], 9);
}

TEST_F(StaticArray, IsAggregateWithNoOverhead)
{
	static_assert(std::is_aggregate_v<hod::StaticArray<int, 3>>);
	static_assert(std::is_trivially_copyable_v<hod::StaticArray<int, 3>>);
	static_assert(sizeof(hod::StaticArray<int, 3>) == sizeof(int[3]));
}

TEST_F(StaticArray, IsConstexpr)
{
	constexpr hod::StaticArray<int, 3> array = {1, 2, 3};
	static_assert(array.Size() == 3);
	static_assert(array[1] == 2);
	static_assert(array.Front() == 1);
	static_assert(array.Back() == 3);
	EXPECT_EQ(array.Size(), 3);
}

// ============================================================================
// Accessors
// ============================================================================

TEST_F(StaticArray, Size)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	EXPECT_EQ(array.Size(), 3);
	EXPECT_EQ((hod::StaticArray<int, 3>::Size()), 3);
}

TEST_F(StaticArray, IndexOperatorAndAt)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	array[1] = 5;
	array.At(2) = 6;
	EXPECT_EQ(array[1], 5);
	EXPECT_EQ(array.At(2), 6);

	const hod::StaticArray<int, 3>& constArray = array;
	EXPECT_EQ(constArray[0], 1);
	EXPECT_EQ(constArray.At(1), 5);
}

TEST_F(StaticArray, FrontBack)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	EXPECT_EQ(array.Front(), 1);
	EXPECT_EQ(array.Back(), 3);
	array.Front() = 7;
	array.Back() = 9;
	EXPECT_EQ(array[0], 7);
	EXPECT_EQ(array[2], 9);
}

TEST_F(StaticArray, Data)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	EXPECT_EQ(array.Data(), &array[0]);
	array.Data()[1] = 8;
	EXPECT_EQ(array[1], 8);
}

TEST_F(StaticArray, RangeBasedFor)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	for (int& value : array)
	{
		value *= 2;
	}
	EXPECT_EQ(array[0], 2);
	EXPECT_EQ(array[1], 4);
	EXPECT_EQ(array[2], 6);

	const hod::StaticArray<int, 3>& constArray = array;
	int                             sum = 0;
	for (int value : constArray)
	{
		sum += value;
	}
	EXPECT_EQ(sum, 12);
}

// ============================================================================
// Mutators
// ============================================================================

TEST_F(StaticArray, Fill)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	array.Fill(7);
	EXPECT_EQ(array[0], 7);
	EXPECT_EQ(array[1], 7);
	EXPECT_EQ(array[2], 7);
}

TEST_F(StaticArray, FillSingleByteType)
{
	hod::StaticArray<uint8_t, 4> bytes = {1, 2, 3, 4};
	bytes.Fill(0xAB);
	for (uint8_t byte : bytes)
	{
		EXPECT_EQ(byte, 0xAB);
	}

	hod::StaticArray<bool, 3> flags = {false, false, false};
	flags.Fill(true);
	EXPECT_TRUE(flags[0] && flags[1] && flags[2]);

	// Filling from one of its own elements must keep working.
	hod::StaticArray<char, 3> chars = {'a', 'b', 'c'};
	chars.Fill(chars[2]);
	EXPECT_EQ(chars[0], 'c');
	EXPECT_EQ(chars[1], 'c');
}

TEST_F(StaticArray, FillIsConstexpr)
{
	constexpr auto filled = []() {
		hod::StaticArray<uint8_t, 3> array = {1, 2, 3};
		array.Fill(9);
		return array;
	}();
	static_assert(filled[0] == 9 && filled[2] == 9);
	EXPECT_EQ(filled[1], 9);
}

TEST_F(StaticArray, Swap)
{
	hod::StaticArray<hod::String, 2> a = {hod::String("a"), hod::String("b")};
	hod::StaticArray<hod::String, 2> b = {hod::String("c"), hod::String("d")};
	a.Swap(b);
	EXPECT_EQ(a[0], "c");
	EXPECT_EQ(a[1], "d");
	EXPECT_EQ(b[0], "a");
	EXPECT_EQ(b[1], "b");
}

// ============================================================================
// ArrayView interop
// ============================================================================

TEST_F(StaticArray, ConvertsToArrayView)
{
	hod::StaticArray<int, 3> array = {1, 2, 3};
	EXPECT_EQ(Sum(array), 6);

	hod::ArrayView view(array);
	static_assert(std::is_same_v<decltype(view), hod::ArrayView<int>>);
	EXPECT_EQ(view.Data(), array.Data());
	EXPECT_EQ(view.Size(), 3);
}
