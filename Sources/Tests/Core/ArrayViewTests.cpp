#include <gtest/gtest.h>
#include <HodEngine/Core/ArrayView.hpp>

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <span>

class ArrayView : public ::testing::Test
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

TEST_F(ArrayView, DefaultConstructor)
{
	hod::ArrayView<int> view;
	EXPECT_EQ(view.Size(), 0);
	EXPECT_TRUE(view.Empty());
	EXPECT_EQ(view.Data(), nullptr);
}

TEST_F(ArrayView, ConstructorFromPointerAndSize)
{
	int                 values[] = {1, 2, 3, 4, 5};
	hod::ArrayView<int> view(values, 3);
	EXPECT_EQ(view.Size(), 3);
	EXPECT_FALSE(view.Empty());
	EXPECT_EQ(view.Data(), values);
}

TEST_F(ArrayView, ConstructorFromCArray)
{
	int                 values[] = {1, 2, 3, 4, 5};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view.Size(), 5);
	EXPECT_EQ(view.Data(), values);
}

TEST_F(ArrayView, ConstructorFromVector)
{
	hod::Vector<int>    vector = {1, 2, 3};
	hod::ArrayView<int> view(vector);
	EXPECT_EQ(view.Size(), vector.Size());
	EXPECT_EQ(view.Data(), vector.Data());
}

TEST_F(ArrayView, ConstructorFromConstVector)
{
	const hod::Vector<int> vector = {1, 2, 3};
	hod::ArrayView<int>    view(vector);
	EXPECT_EQ(view.Size(), 3);
	EXPECT_EQ(view.Data(), vector.Data());
}

TEST_F(ArrayView, ConstructorFromVectorOfNonTrivialType)
{
	hod::Vector<hod::String>    vector = {hod::String("hello"), hod::String("world")};
	hod::ArrayView<hod::String> view(vector);
	EXPECT_EQ(view.Size(), 2);
	EXPECT_EQ(view[1], "world");
}

TEST_F(ArrayView, ImplicitConversionAsFunctionArgument)
{
	hod::Vector<int> vector = {1, 2, 3};
	int              values[] = {4, 5, 6};
	EXPECT_EQ(Sum(vector), 6);
	EXPECT_EQ(Sum(values), 15);
}

TEST_F(ArrayView, DeductionGuides)
{
	hod::Vector<int> vector = {1, 2, 3};
	int              values[] = {4, 5, 6};

	hod::ArrayView fromVector(vector);
	hod::ArrayView fromArray(values);
	hod::ArrayView fromPointer(values, 2);

	static_assert(std::is_same_v<decltype(fromVector), hod::ArrayView<int>>);
	static_assert(std::is_same_v<decltype(fromArray), hod::ArrayView<int>>);
	static_assert(std::is_same_v<decltype(fromPointer), hod::ArrayView<int>>);

	EXPECT_EQ(fromVector.Size(), 3);
	EXPECT_EQ(fromArray.Size(), 3);
	EXPECT_EQ(fromPointer.Size(), 2);
}

TEST_F(ArrayView, ElementTypeMustMatchExactly)
{
	static_assert(std::is_constructible_v<hod::ArrayView<int>, const hod::Vector<int>&>);
	static_assert(!std::is_constructible_v<hod::ArrayView<int>, const hod::Vector<float>&>);
	static_assert(!std::is_constructible_v<hod::ArrayView<int>, std::nullptr_t>);
}

TEST_F(ArrayView, InteropWithStdSpanIsManual)
{
	// No conversion helper on purpose: build a std::span yourself at the call site when actually needed.
	int                  values[] = {1, 2, 3};
	std::span<const int> stdSpan(values);
	hod::ArrayView<int>  view(stdSpan.data(), static_cast<uint32_t>(stdSpan.size()));
	EXPECT_EQ(view.Size(), 3);

	std::span<const int> backToStd(view.Data(), view.Size());
	EXPECT_EQ(backToStd.size(), 3);
}

TEST_F(ArrayView, CopyIsShallow)
{
	int                 values[] = {1, 2, 3};
	hod::ArrayView<int> view(values);
	hod::ArrayView<int> copy(view);
	EXPECT_EQ(copy.Data(), view.Data());
	EXPECT_EQ(copy.Size(), view.Size());
}

TEST_F(ArrayView, IsConstexprConstructible)
{
	static constexpr int          values[] = {1, 2, 3};
	constexpr hod::ArrayView<int> view(values);
	static_assert(view.Size() == 3);
	static_assert(view[1] == 2);
	static_assert(view.Find(3) == 2);
	EXPECT_EQ(view.Size(), 3);
}

// ============================================================================
// Accessors
// ============================================================================

TEST_F(ArrayView, IndexOperator)
{
	int                 values[] = {1, 2, 3};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view[0], 1);
	EXPECT_EQ(view[2], 3);
}

TEST_F(ArrayView, At)
{
	int                 values[] = {1, 2, 3};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view.At(1), 2);
}

TEST_F(ArrayView, FrontBack)
{
	int                 values[] = {1, 2, 3};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view.Front(), 1);
	EXPECT_EQ(view.Back(), 3);
}

TEST_F(ArrayView, ElementsAreReadOnly)
{
	static_assert(std::is_same_v<decltype(std::declval<hod::ArrayView<int>>()[0]), const int&>);
	static_assert(std::is_same_v<decltype(std::declval<hod::ArrayView<int>>().Data()), const int*>);
}

TEST_F(ArrayView, RangeBasedFor)
{
	int                 values[] = {1, 2, 3};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(Sum(view), 6);
}

// ============================================================================
// Mutators (view rebinding, not the underlying data)
// ============================================================================

TEST_F(ArrayView, Swap)
{
	int                 first[] = {1, 2, 3};
	int                 second[] = {4, 5};
	hod::ArrayView<int> a(first);
	hod::ArrayView<int> b(second);
	a.Swap(b);
	EXPECT_EQ(a.Data(), second);
	EXPECT_EQ(a.Size(), 2);
	EXPECT_EQ(b.Data(), first);
	EXPECT_EQ(b.Size(), 3);
}

TEST_F(ArrayView, SubView)
{
	int                 values[] = {1, 2, 3, 4, 5};
	hod::ArrayView<int> view(values);

	hod::ArrayView<int> tail = view.SubView(3);
	EXPECT_EQ(tail.Size(), 2);
	EXPECT_EQ(tail.Data(), values + 3);

	hod::ArrayView<int> head = view.SubView(0, 2);
	EXPECT_EQ(head.Size(), 2);
	EXPECT_EQ(head.Data(), values);
}

TEST_F(ArrayView, SubViewClampsCount)
{
	int                 values[] = {1, 2, 3, 4, 5};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view.SubView(2, 100).Size(), 3);
	EXPECT_TRUE(view.SubView(5).Empty());
}

// ============================================================================
// Find / Contains
// ============================================================================

TEST_F(ArrayView, Find)
{
	int                 values[] = {1, 2, 3, 2, 1};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view.Find(2), 1);
	EXPECT_EQ(view.Find(2, 2), 3);
	EXPECT_EQ(view.Find(9), hod::ArrayView<int>::Npos);
	EXPECT_EQ(view.Find(1, 100), hod::ArrayView<int>::Npos);
}

TEST_F(ArrayView, FindR)
{
	int                 values[] = {1, 2, 3, 2, 1};
	hod::ArrayView<int> view(values);
	EXPECT_EQ(view.FindR(2), 3);
	EXPECT_EQ(view.FindR(2, 2), 1);
	EXPECT_EQ(view.FindR(9), hod::ArrayView<int>::Npos);
	EXPECT_EQ(hod::ArrayView<int>().FindR(1), hod::ArrayView<int>::Npos);
}

TEST_F(ArrayView, Contains)
{
	int                 values[] = {1, 2, 3};
	hod::ArrayView<int> view(values);
	EXPECT_TRUE(view.Contains(3));
	EXPECT_FALSE(view.Contains(4));
}
