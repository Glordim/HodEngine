#include <gtest/gtest.h>

#include <algorithm>

#include <HodEngine/Core/Random.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::random_tests
{
	TEST(Random, SameSeedSameSequence)
	{
		Random first(42);
		Random second(42);
		for (int index = 0; index < 100; ++index)
		{
			EXPECT_EQ(first.NextUInt32(), second.NextUInt32());
		}
	}

	TEST(Random, SetSeedStartsAgain)
	{
		Random   random(7);
		uint32_t firstValue = random.NextUInt32();
		random.NextUInt32();

		random.SetSeed(7);
		EXPECT_EQ(random.NextUInt32(), firstValue);
	}

	TEST(Random, DifferentSeedsDifferentSequences)
	{
		Random first(1);
		Random second(2);
		int    sameCount = 0;
		for (int index = 0; index < 100; ++index)
		{
			sameCount += (first.NextUInt32() == second.NextUInt32()) ? 1 : 0;
		}
		EXPECT_LT(sameCount, 3);
	}

	TEST(Random, IntRangeBounds)
	{
		Random random(3);
		bool   minSeen = false;
		bool   maxSeen = false;
		for (int index = 0; index < 1000; ++index)
		{
			int32_t value = random.Range(-3, 4);
			EXPECT_GE(value, -3);
			EXPECT_LT(value, 4);
			minSeen |= (value == -3);
			maxSeen |= (value == 3);
		}
		EXPECT_TRUE(minSeen);
		EXPECT_TRUE(maxSeen);

		EXPECT_EQ(random.Range(5, 5), 5); // Empty
		EXPECT_EQ(random.Range(5, 2), 5);

		int32_t whole = random.Range(INT32_MIN, INT32_MAX); // No overflow
		EXPECT_LT(whole, INT32_MAX);
	}

	TEST(Random, FloatRangeBounds)
	{
		Random random(4);
		for (int index = 0; index < 1000; ++index)
		{
			float value = random.Range(-1.0f, 2.0f);
			EXPECT_GE(value, -1.0f);
			EXPECT_LT(value, 2.0f);

			float value01 = random.Value01();
			EXPECT_GE(value01, 0.0f);
			EXPECT_LT(value01, 1.0f);
		}
	}

	TEST(Random, IndexIsUniform)
	{
		Random                     random(5);
		constexpr uint32_t         count = 6;
		constexpr int              draws = 60000;
		StaticArray<int, count>    histogram = {};
		for (int index = 0; index < draws; ++index)
		{
			uint32_t value = random.Index(count);
			ASSERT_LT(value, count);
			++histogram[value];
		}
		for (int hits : histogram)
		{
			EXPECT_NEAR(hits, draws / count, draws / count / 10); // Within 10%
		}
	}

	TEST(Random, ChanceExtremes)
	{
		Random random(6);
		int    hits = 0;
		for (int index = 0; index < 1000; ++index)
		{
			EXPECT_FALSE(random.Chance(0.0f));
			EXPECT_TRUE(random.Chance(1.0f));
			hits += random.Chance(0.25f) ? 1 : 0;
		}
		EXPECT_NEAR(hits, 250, 60);
	}

	TEST(Random, Pick)
	{
		Random      random(8);
		Vector<int> values = {1, 2, 3};

		bool seen[3] = {};
		for (int index = 0; index < 100; ++index)
		{
			int picked = random.Pick(values);
			ASSERT_GE(picked, 1);
			ASSERT_LE(picked, 3);
			seen[picked - 1] = true;
		}
		EXPECT_TRUE(seen[0] && seen[1] && seen[2]);

		int& element = random.Pick(values); // A reference to the element
		element = 10;
		EXPECT_TRUE(values[0] == 10 || values[1] == 10 || values[2] == 10);
	}

	TEST(Random, ShuffleKeepsTheElements)
	{
		Random      random(9);
		Vector<int> values;
		for (int value = 0; value < 50; ++value)
		{
			values.PushBack(value);
		}

		random.Shuffle(values);

		int  sum = 0;
		bool moved = false;
		for (uint32_t index = 0; index < values.Size(); ++index)
		{
			sum += values[index];
			moved |= (values[index] != static_cast<int>(index));
		}
		EXPECT_EQ(sum, 49 * 50 / 2);
		EXPECT_TRUE(moved);

		Vector<int> sorted = values;
		std::sort(sorted.begin(), sorted.end());
		for (uint32_t index = 0; index < sorted.Size(); ++index)
		{
			EXPECT_EQ(sorted[index], static_cast<int>(index));
		}
	}

	TEST(Random, ShuffleIsUniform)
	{
		// The 6 orders of 3 elements, as likely
		Random             random(10);
		constexpr int      draws = 60000;
		StaticArray<int, 6> histogram = {};
		for (int index = 0; index < draws; ++index)
		{
			StaticArray<int, 3> values = {0, 1, 2};
			random.Shuffle(values);
			++histogram[values[0] * 2 + (values[1] > values[2] ? 1 : 0)];
		}
		for (int hits : histogram)
		{
			EXPECT_NEAR(hits, draws / 6, draws / 6 / 10);
		}
	}

	TEST(Random, DefaultSeededEachRun)
	{
		Random first;
		Random second;
		EXPECT_NE(first.NextUInt32() ^ (static_cast<uint64_t>(first.NextUInt32()) << 32), second.NextUInt32() ^ (static_cast<uint64_t>(second.NextUInt32()) << 32));

		Random& defaultRandom = Random::GetDefault();
		EXPECT_EQ(&defaultRandom, &Random::GetDefault());
	}
}
