#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Assert.hpp"

#include <cstdint>
#include <utility>

namespace hod::inline core
{
	/// @brief A pseudo random generator (PCG32): fast, small, the same sequence for the same seed (a level generated again, a replay).
	/// A gameplay wanting its sequence reproducible holds its own instance with its seed, otherwise uses GetDefault(). Not thread safe:
	/// an instance per thread (GetDefault is)
	class HOD_CORE_API Random
	{
	public:
		/// @brief Seeded differently each time (from the system's entropy)
		Random();
		explicit Random(uint64_t seed);

	public:
		/// @brief Its sequence starts again: the same seed gives the same sequence
		void		SetSeed(uint64_t seed);

		/// @return Uniform over the whole range of uint32_t
		uint32_t	NextUInt32();

		/// @return Uniform in [0, count), unbiased. count must not be 0 (0 then)
		uint32_t	Index(uint32_t count);
		/// @return Uniform in [min, maxExclusive), unbiased. min when the range is empty
		int32_t		Range(int32_t min, int32_t maxExclusive);
		/// @return Uniform in [min, max)
		float		Range(float min, float max);
		/// @return Uniform in [0, 1)
		float		Value01();
		/// @return true with this probability (0: never, 1: always)
		bool		Chance(float probability);

		/// @return One of its elements, uniform. Not empty (asserted)
		template<typename Container>
		decltype(auto)	Pick(Container& container);
		/// @brief Its elements in a random order (Fisher-Yates), every order as likely
		template<typename Container>
		void			Shuffle(Container& container);

		/// @return The calling thread's instance, seeded differently each run
		static Random&	GetDefault();

	private:
		uint64_t	_state = 0;
	};

	/// @brief
	/// @tparam Container A Vector, a StaticArray, an ArrayView...
	/// @param container
	/// @return
	template<typename Container>
	decltype(auto) Random::Pick(Container& container)
	{
		Assert(container.Size() > 0);
		return container[Index(container.Size())];
	}

	/// @brief
	/// @tparam Container A Vector, a StaticArray...
	/// @param container
	template<typename Container>
	void Random::Shuffle(Container& container)
	{
		for (uint32_t index = container.Size(); index > 1; --index)
		{
			uint32_t other = Index(index);
			if (other != index - 1)
			{
				std::swap(container[index - 1], container[other]);
			}
		}
	}
}
