#pragma once

#include <cstdint>

constexpr float pi = 3.14159265358979323846f;

namespace hod::math
{
	inline constexpr float DegreeToRadian(float degree)
	{
		return degree * (pi / 180.0f);
	}

	inline constexpr float RadianToDegree(float radian)
	{
		return radian * (180.0f / pi);
	}
}
