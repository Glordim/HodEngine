#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Random.hpp"

#include <random>

namespace hod::inline core
{
	namespace
	{
		constexpr uint64_t Multiplier = 6364136223846793005ull;
		constexpr uint64_t Increment = 1442695040888963407ull; // Odd: one sequence, the seed choosing where it starts
	}

	/// @brief
	Random::Random()
	{
		std::random_device device;
		SetSeed((static_cast<uint64_t>(device()) << 32) | device());
	}

	/// @brief
	/// @param seed
	Random::Random(uint64_t seed)
	{
		SetSeed(seed);
	}

	/// @brief As pcg32_srandom
	/// @param seed
	void Random::SetSeed(uint64_t seed)
	{
		_state = 0;
		NextUInt32();
		_state += seed;
		NextUInt32();
	}

	/// @brief PCG-XSH-RR
	/// @return
	uint32_t Random::NextUInt32()
	{
		uint64_t previous = _state;
		_state = previous * Multiplier + Increment;
		uint32_t xorShifted = static_cast<uint32_t>(((previous >> 18u) ^ previous) >> 27u);
		uint32_t rotation = static_cast<uint32_t>(previous >> 59u);
		return (xorShifted >> rotation) | (xorShifted << ((0u - rotation) & 31u));
	}

	/// @brief The values below 2^32 % count rejected: what is left is a multiple of count
	/// @param count
	/// @return
	uint32_t Random::Index(uint32_t count)
	{
		if (count == 0)
		{
			Assert(false);
			return 0;
		}

		uint32_t threshold = (0u - count) % count;
		while (true)
		{
			uint32_t value = NextUInt32();
			if (value >= threshold)
			{
				return value % count;
			}
		}
	}

	/// @brief
	/// @param min
	/// @param maxExclusive
	/// @return
	int32_t Random::Range(int32_t min, int32_t maxExclusive)
	{
		if (maxExclusive <= min)
		{
			return min;
		}
		uint32_t count = static_cast<uint32_t>(static_cast<int64_t>(maxExclusive) - static_cast<int64_t>(min));
		return static_cast<int32_t>(static_cast<int64_t>(min) + Index(count));
	}

	/// @brief
	/// @param min
	/// @param max
	/// @return
	float Random::Range(float min, float max)
	{
		float value = min + (max - min) * Value01();
		return (value < max || max <= min) ? value : min; // Rounded up to max: never returned
	}

	/// @brief The 24 high bits: every float of [0, 1) with this step, as likely
	/// @return
	float Random::Value01()
	{
		return static_cast<float>(NextUInt32() >> 8) * (1.0f / 16777216.0f);
	}

	/// @brief
	/// @param probability
	/// @return
	bool Random::Chance(float probability)
	{
		return Value01() < probability;
	}

	/// @brief
	/// @return
	Random& Random::GetDefault()
	{
		thread_local Random random;
		return random;
	}
}
