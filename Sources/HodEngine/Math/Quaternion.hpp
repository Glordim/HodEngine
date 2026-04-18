#pragma once
#include "HodEngine/Math/Export.hpp"

#include "HodEngine/Math/Vector2.hpp"

#include <cstdint>

namespace hod::inline math
{
	class HOD_MATH_API Quaternion final
	{
	public:
		static const Quaternion Identity;

	public:
		Quaternion();
		Quaternion(float x, float y, float z, float w);

		float GetAngleZ() const;

		static Quaternion LookRotation(const Vector2& direction);

		Vector2    operator*(const Vector2& vec) const;
		Quaternion operator*(const Quaternion& q) const;

	public:
		float _x;
		float _y;
		float _z;
		float _w;
	};
}
