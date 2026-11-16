#pragma once

#include <limits>

namespace hod::inline core
{
	template<typename T>
	constexpr T Min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	constexpr T Max(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template<typename T>
	constexpr T Clamp(T value, T min, T max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	}

	template<typename T>
	inline constexpr T Infinity = std::numeric_limits<T>::infinity();
}
