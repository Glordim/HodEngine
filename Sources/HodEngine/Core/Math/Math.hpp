#pragma once

#include <cstdint>

namespace hod::math
{
	constexpr float pi = 3.14159265358979323846f;

	inline constexpr float DegreeToRadian(float degree)
	{
		return degree * (pi / 180.0f);
	}

	inline constexpr float RadianToDegree(float radian)
	{
		return radian * (180.0f / pi);
	}

	template<typename T>
	inline constexpr T Min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	inline constexpr T Max(T a, T b)
	{
		return (a > b) ? a : b;
	}
}
