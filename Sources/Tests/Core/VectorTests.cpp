#include <gtest/gtest.h>

#include <HodEngine/Core/Vector.hpp>

namespace hod
{
	TEST(Vector, Ctor)
	{
		Vector<uint32_t> vec;
		EXPECT_EQ(vec.Size(), 0);
		EXPECT_EQ(vec.Capacity(), 0);
		EXPECT_TRUE(vec.Empty());
	}
}
