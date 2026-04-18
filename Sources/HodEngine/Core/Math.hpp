#pragma once

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
}
