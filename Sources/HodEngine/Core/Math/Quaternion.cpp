#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Math/Quaternion.hpp"

#include <cstring>
#include <cmath>

namespace hod
{
	const Quaternion Quaternion::Identity;

	/// @brief 
	Quaternion::Quaternion()
	: _x(0.0f)
	, _y(0.0f)
	, _z(0.0f)
	, _w(1.0f)
	{

	}

	/// @brief 
	Quaternion::Quaternion(float x, float y, float z, float w)
	: _x(x)
	, _y(y)
	, _z(z)
	, _w(w)
	{

	}

	/// @brief 
	/// @return 
	float Quaternion::GetAngleZ() const
	{
		return atan2(2.0f * (_w * _z + _x * _y), 1.0f - 2.0f * (_y * _y + _z * _z));
	}
}
