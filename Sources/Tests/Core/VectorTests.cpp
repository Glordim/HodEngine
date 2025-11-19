#include <gtest/gtest.h>
#include <HodEngine/Core/Vector.hpp>
#include <memory>
#include <string>

// ============================================================================
// Base Fixture pour tous les tests Vector
// ============================================================================

class Vector : public ::testing::Test
{
protected:
	void SetUp() override {}

	void TearDown() override {}
};

// ============================================================================
// Construction & Destruction Tests
// ============================================================================

TEST_F(Vector, DefaultConstructor)
{
	hod::Vector<int> vec;
	EXPECT_EQ(vec.Size(), 0);
	EXPECT_EQ(vec.Capacity(), 0);
	EXPECT_TRUE(vec.Empty());
	EXPECT_EQ(vec.Data(), nullptr);
}

TEST_F(Vector, ConstructorWithCount)
{
	hod::Vector<int> vec(5);
	EXPECT_EQ(vec.Size(), 5);
	EXPECT_GE(vec.Capacity(), 5);
	EXPECT_FALSE(vec.Empty());

	for (uint32_t i = 0; i < vec.Size(); ++i)
	{
		EXPECT_EQ(vec[i], 0);
	}
}

TEST_F(Vector, ConstructorWithInitializerList)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};
	EXPECT_EQ(vec.Size(), 5);
	EXPECT_GE(vec.Capacity(), 5);

	for (uint32_t i = 0; i < vec.Size(); ++i)
	{
		EXPECT_EQ(vec[i], static_cast<int>(i + 1));
	}
}

TEST_F(Vector, CopyConstructor)
{
	hod::Vector<int> original = {1, 2, 3, 4, 5};
	hod::Vector<int> copy(original);

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_GE(copy.Capacity(), original.Size());

	for (uint32_t i = 0; i < copy.Size(); ++i)
	{
		EXPECT_EQ(copy[i], original[i]);
	}

	// Verify deep copy
	copy[0] = 99;
	EXPECT_NE(copy[0], original[0]);
}

TEST_F(Vector, MoveConstructor)
{
	hod::Vector<int> original = {1, 2, 3, 4, 5};
	const uint32_t   originalSize = original.Size();
	const int*       originalData = original.Data();

	hod::Vector<int> moved(std::move(original));

	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_EQ(moved.Data(), originalData);
	EXPECT_EQ(original.Size(), 0);
	EXPECT_EQ(original.Capacity(), 0);
	EXPECT_EQ(original.Data(), nullptr);

	for (uint32_t i = 0; i < moved.Size(); ++i)
	{
		EXPECT_EQ(moved[i], static_cast<int>(i + 1));
	}
}

// ============================================================================
// Assignment Operators Tests
// ============================================================================

TEST_F(Vector, CopyAssignment)
{
	hod::Vector<int> original = {1, 2, 3, 4, 5};
	hod::Vector<int> copy = {10, 20};

	copy = original;

	EXPECT_EQ(copy.Size(), original.Size());
	for (uint32_t i = 0; i < copy.Size(); ++i)
	{
		EXPECT_EQ(copy[i], original[i]);
	}

	// Verify deep copy
	copy[0] = 99;
	EXPECT_NE(copy[0], original[0]);
}

TEST_F(Vector, SelfAssignment)
{
	hod::Vector<int> vec = {1, 2, 3};

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
	vec = vec;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[1], 2);
	EXPECT_EQ(vec[2], 3);
}

TEST_F(Vector, MoveAssignment)
{
	hod::Vector<int> original = {1, 2, 3, 4, 5};
	hod::Vector<int> target = {10, 20};

	const uint32_t originalSize = original.Size();
	const int*     originalData = original.Data();

	target = std::move(original);

	EXPECT_EQ(target.Size(), originalSize);
	EXPECT_EQ(target.Data(), originalData);
	EXPECT_EQ(original.Size(), 0);

	for (uint32_t i = 0; i < target.Size(); ++i)
	{
		EXPECT_EQ(target[i], static_cast<int>(i + 1));
	}
}

TEST_F(Vector, InitializerListAssignment)
{
	hod::Vector<int> vec = {1, 2, 3};
	vec = {10, 20, 30, 40};

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[3], 40);
}

// ============================================================================
// Element Access Tests
// ============================================================================

TEST_F(Vector, OperatorBracket)
{
	hod::Vector<int> vec = {10, 20, 30};

	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);

	vec[1] = 99;
	EXPECT_EQ(vec[1], 99);
}

TEST_F(Vector, AtMethod)
{
	hod::Vector<int> vec = {10, 20, 30};

	EXPECT_EQ(vec.At(0), 10);
	EXPECT_EQ(vec.At(2), 30);

	vec.At(1) = 99;
	EXPECT_EQ(vec.At(1), 99);
}

TEST_F(Vector, FrontAndBack)
{
	hod::Vector<int> vec = {10, 20, 30, 40};

	EXPECT_EQ(vec.Front(), 10);
	EXPECT_EQ(vec.Back(), 40);

	vec.Front() = 99;
	vec.Back() = 88;

	EXPECT_EQ(vec.Front(), 99);
	EXPECT_EQ(vec.Back(), 88);
}

TEST_F(Vector, DataAccess)
{
	hod::Vector<int> vec = {1, 2, 3};
	int*             data = vec.Data();

	EXPECT_NE(data, nullptr);
	EXPECT_EQ(data[0], 1);
	EXPECT_EQ(data[1], 2);
	EXPECT_EQ(data[2], 3);
}

// ============================================================================
// Iterator Tests
// ============================================================================

TEST_F(Vector, BasicIterators)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	int sum = 0;
	for (auto it = vec.Begin(); it != vec.End(); ++it)
	{
		sum += *it;
	}
	EXPECT_EQ(sum, 15);
}

TEST_F(Vector, ConstIterators)
{
	const hod::Vector<int> vec = {1, 2, 3, 4, 5};

	int sum = 0;
	for (auto it = vec.CBegin(); it != vec.CEnd(); ++it)
	{
		sum += *it;
	}
	EXPECT_EQ(sum, 15);
}

TEST_F(Vector, RangeBasedFor)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	int sum = 0;
	for (int val : vec)
	{
		sum += val;
	}
	EXPECT_EQ(sum, 15);
}

TEST_F(Vector, ReverseIterators)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	hod::Vector<int> reversed;
	for (auto it = vec.RBegin(); it != vec.REnd(); ++it)
	{
		reversed.PushBack(*it);
	}

	EXPECT_EQ(reversed.Size(), 5);
	EXPECT_EQ(reversed[0], 5);
	EXPECT_EQ(reversed[4], 1);
}

TEST_F(Vector, IteratorArithmetic)
{
	hod::Vector<int> vec = {10, 20, 30, 40, 50};

	auto it = vec.Begin();
	EXPECT_EQ(*it, 10);

	it += 2;
	EXPECT_EQ(*it, 30);

	it -= 1;
	EXPECT_EQ(*it, 20);

	auto it2 = it + 3;
	EXPECT_EQ(*it2, 50);
}

// ============================================================================
// Capacity Tests
// ============================================================================

TEST_F(Vector, Reserve)
{
	hod::Vector<int> vec;
	vec.Reserve(100);

	EXPECT_GE(vec.Capacity(), 100);
	EXPECT_EQ(vec.Size(), 0);

	for (int i = 0; i < 50; ++i)
	{
		vec.PushBack(i);
	}

	EXPECT_EQ(vec.Size(), 50);
	EXPECT_GE(vec.Capacity(), 100);
}

TEST_F(Vector, ShrinkToFit)
{
	hod::Vector<int> vec;
	vec.Reserve(100);

	for (int i = 0; i < 10; ++i)
	{
		vec.PushBack(i);
	}

	EXPECT_GE(vec.Capacity(), 100);

	vec.ShrinkToFit();

	EXPECT_EQ(vec.Capacity(), vec.Size());
	EXPECT_EQ(vec.Size(), 10);
}

TEST_F(Vector, Clear)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};
	uint32_t         oldCapacity = vec.Capacity();

	vec.Clear();

	EXPECT_EQ(vec.Size(), 0);
	EXPECT_TRUE(vec.Empty());
	EXPECT_EQ(vec.Capacity(), oldCapacity); // Capacity unchanged
}

TEST_F(Vector, ResizeGrow)
{
	hod::Vector<int> vec = {1, 2, 3};
	vec.Resize(5);

	EXPECT_EQ(vec.Size(), 5);
	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[2], 3);
	EXPECT_EQ(vec[3], 0);
	EXPECT_EQ(vec[4], 0);
}

TEST_F(Vector, ResizeShrink)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};
	vec.Resize(3);

	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[2], 3);
}

TEST_F(Vector, ResizeWithValue)
{
	hod::Vector<int> vec = {1, 2, 3};
	vec.Resize(5, 99);

	EXPECT_EQ(vec.Size(), 5);
	EXPECT_EQ(vec[3], 99);
	EXPECT_EQ(vec[4], 99);
}

// ============================================================================
// Modifiers Tests
// ============================================================================

TEST_F(Vector, PushBack)
{
	hod::Vector<int> vec;

	for (int i = 0; i < 10; ++i)
	{
		vec.PushBack(i);
	}

	EXPECT_EQ(vec.Size(), 10);
	for (uint32_t i = 0; i < vec.Size(); ++i)
	{
		EXPECT_EQ(vec[i], static_cast<int>(i));
	}
}

TEST_F(Vector, PushBackRvalue)
{
	hod::Vector<std::string> vec;
	std::string              str = "test";

	vec.PushBack(std::move(str));

	EXPECT_EQ(vec.Size(), 1);
	EXPECT_EQ(vec[0], "test");
}

TEST_F(Vector, PopBack)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	vec.PopBack();

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec.Back(), 4);

	vec.PopBack();
	vec.PopBack();

	EXPECT_EQ(vec.Size(), 2);
	EXPECT_EQ(vec.Back(), 2);
}

TEST_F(Vector, EmplaceBack)
{
	hod::Vector<std::pair<int, std::string>> vec;

	vec.EmplaceBack(1, "first");
	vec.EmplaceBack(2, "second");

	EXPECT_EQ(vec.Size(), 2);
	EXPECT_EQ(vec[0].first, 1);
	EXPECT_EQ(vec[0].second, "first");
	EXPECT_EQ(vec[1].first, 2);
	EXPECT_EQ(vec[1].second, "second");
}

TEST_F(Vector, InsertSingleElement)
{
	hod::Vector<int> vec = {1, 2, 4, 5};

	auto it = vec.Begin() + 2;
	vec.Insert(it, 3);

	EXPECT_EQ(vec.Size(), 5);
	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[2], 3);
	EXPECT_EQ(vec[4], 5);
}

TEST_F(Vector, InsertAtBeginning)
{
	hod::Vector<int> vec = {2, 3, 4};
	vec.Insert(vec.Begin(), 1);

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[3], 4);
}

TEST_F(Vector, InsertAtEnd)
{
	hod::Vector<int> vec = {1, 2, 3};
	vec.Insert(vec.End(), 4);

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec[3], 4);
}

TEST_F(Vector, InsertRange)
{
	hod::Vector<int> vec = {1, 5};
	hod::Vector<int> toInsert = {2, 3, 4};

	vec.Insert(vec.Begin() + 1, toInsert.Begin(), toInsert.End());

	EXPECT_EQ(vec.Size(), 5);
	for (uint32_t i = 0; i < vec.Size(); ++i)
	{
		EXPECT_EQ(vec[i], static_cast<int>(i + 1));
	}
}

TEST_F(Vector, Emplace)
{
	hod::Vector<std::pair<int, std::string>> vec;
	vec.EmplaceBack(1, "first");
	vec.EmplaceBack(3, "third");

	auto it = vec.Begin() + 1;
	vec.Emplace(it, 2, "second");

	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec[1].first, 2);
	EXPECT_EQ(vec[1].second, "second");
}

TEST_F(Vector, EraseSingleElement)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	auto it = vec.Begin() + 2;
	vec.Erase(it);

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec[2], 4);
}

TEST_F(Vector, EraseRange)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	vec.Erase(vec.Begin() + 1, vec.Begin() + 4);

	EXPECT_EQ(vec.Size(), 2);
	EXPECT_EQ(vec[0], 1);
	EXPECT_EQ(vec[1], 5);
}

TEST_F(Vector, EraseByIndex)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};
	vec.Erase(2);

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec[2], 4);
}

// ============================================================================
// Swap Tests
// ============================================================================

TEST_F(Vector, SwapVectors)
{
	hod::Vector<int> vec1 = {1, 2, 3};
	hod::Vector<int> vec2 = {10, 20, 30, 40};

	vec1.Swap(vec2);

	EXPECT_EQ(vec1.Size(), 4);
	EXPECT_EQ(vec1[0], 10);
	EXPECT_EQ(vec2.Size(), 3);
	EXPECT_EQ(vec2[0], 1);
}

TEST_F(Vector, SwapElements)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	vec.Swap(vec.Begin(), vec.Begin() + 4);

	EXPECT_EQ(vec[0], 5);
	EXPECT_EQ(vec[4], 1);
}

TEST_F(Vector, SwapByIndex)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};
	vec.Swap(0, 4);

	EXPECT_EQ(vec[0], 5);
	EXPECT_EQ(vec[4], 1);
}

TEST_F(Vector, SwapAndPopBack)
{
	hod::Vector<int> vec = {1, 2, 3, 4, 5};

	vec.SwapAndPopBack(vec.Begin() + 1);

	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec[1], 5);
}

// ============================================================================
// Comparison Tests
// ============================================================================

TEST_F(Vector, EqualityOperator)
{
	hod::Vector<int> vec1 = {1, 2, 3, 4, 5};
	hod::Vector<int> vec2 = {1, 2, 3, 4, 5};
	hod::Vector<int> vec3 = {1, 2, 3, 4, 6};
	hod::Vector<int> vec4 = {1, 2, 3};

	EXPECT_TRUE(vec1 == vec2);
	EXPECT_FALSE(vec1 == vec3);
	EXPECT_FALSE(vec1 == vec4);
}

// ============================================================================
// Complex Types Tests
// ============================================================================

struct NonTrivial
{
	static int constructCount;
	static int destructCount;

	int value;

	NonTrivial(int v = 0)
	: value(v)
	{
		++constructCount;
	}

	NonTrivial(const NonTrivial& other)
	: value(other.value)
	{
		++constructCount;
	}

	NonTrivial(NonTrivial&& other) noexcept
	: value(other.value)
	{
		++constructCount;
	}

	~NonTrivial()
	{
		++destructCount;
	}

	NonTrivial& operator=(const NonTrivial& other)
	{
		value = other.value;
		return *this;
	}

	NonTrivial& operator=(NonTrivial&& other) noexcept
	{
		value = other.value;
		return *this;
	}

	bool operator==(const NonTrivial& other) const
	{
		return value == other.value;
	}

	bool operator!=(const NonTrivial& other) const
	{
		return value != other.value;
	}

	static void Reset()
	{
		constructCount = 0;
		destructCount = 0;
	}
};

int NonTrivial::constructCount = 0;
int NonTrivial::destructCount = 0;

TEST_F(Vector, NonTrivialType)
{
	NonTrivial::Reset();

	{
		hod::Vector<NonTrivial> vec;
		vec.PushBack(NonTrivial(1));
		vec.PushBack(NonTrivial(2));
		vec.PushBack(NonTrivial(3));

		EXPECT_EQ(vec.Size(), 3);
		EXPECT_EQ(vec[0].value, 1);
	}

	EXPECT_EQ(NonTrivial::constructCount, NonTrivial::destructCount);
}

TEST_F(Vector, StringVector)
{
	hod::Vector<std::string> vec;
	vec.PushBack("hello");
	vec.PushBack("world");
	vec.EmplaceBack("!");

	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec[0], "hello");
	EXPECT_EQ(vec[1], "world");
	EXPECT_EQ(vec[2], "!");
}

// ============================================================================
// Edge Cases Tests
// ============================================================================

TEST_F(Vector, EmptyVectorOperations)
{
	hod::Vector<int> vec;

	EXPECT_TRUE(vec.Empty());
	EXPECT_EQ(vec.Size(), 0);
	EXPECT_EQ(vec.Begin(), vec.End());

	vec.Clear();
	EXPECT_TRUE(vec.Empty());
}

TEST_F(Vector, SingleElementVector)
{
	hod::Vector<int> vec = {42};

	EXPECT_EQ(vec.Size(), 1);
	EXPECT_EQ(vec.Front(), 42);
	EXPECT_EQ(vec.Back(), 42);
	EXPECT_EQ(vec[0], 42);

	vec.PopBack();
	EXPECT_TRUE(vec.Empty());
}

TEST_F(Vector, LargeVector)
{
	hod::Vector<int> vec;
	const int        count = 10000;

	for (int i = 0; i < count; ++i)
	{
		vec.PushBack(i);
	}

	EXPECT_EQ(vec.Size(), count);

	for (int i = 0; i < count; ++i)
	{
		EXPECT_EQ(vec[i], i);
	}
}

TEST_F(Vector, RepeatedPushPopOperations)
{
	hod::Vector<int> vec;

	for (int cycle = 0; cycle < 100; ++cycle)
	{
		for (int i = 0; i < 10; ++i)
		{
			vec.PushBack(i);
		}

		for (int i = 0; i < 5; ++i)
		{
			vec.PopBack();
		}
	}

	EXPECT_EQ(vec.Size(), 500);
}

// ============================================================================
// Memory Management Tests
// ============================================================================

TEST_F(Vector, CapacityGrowth)
{
	hod::Vector<int>      vec;
	hod::Vector<uint32_t> capacities;

	for (int i = 0; i < 100; ++i)
	{
		uint32_t oldCap = vec.Capacity();
		vec.PushBack(i);
		uint32_t newCap = vec.Capacity();

		if (newCap > oldCap)
		{
			capacities.PushBack(newCap);
		}
	}

	// Verify exponential growth
	EXPECT_GT(capacities.Size(), 0);
}

TEST_F(Vector, NoUnnecessaryReallocations)
{
	hod::Vector<int> vec;
	vec.Reserve(100);

	const int* dataPtr = vec.Data();

	for (int i = 0; i < 100; ++i)
	{
		vec.PushBack(i);
	}

	// Data pointer should not change if capacity was sufficient
	EXPECT_EQ(vec.Data(), dataPtr);
}
