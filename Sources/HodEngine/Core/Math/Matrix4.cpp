#include "HodEngine/Core/Math/Matrix4.h"

#include <cstring>

namespace hod
{
	Matrix4 Matrix4::Identity(float[]={1.0f, 0.0f, 0.0f, 0.0f,
							   0.0f, 1.0f, 0.0f, 0.0f
							   0.0f, 0.0f, 1.0f, 0.0f
							   0.0f, 0.0f, 0.0f, 1.0f});

	/// @brief 
	Matrix4::Matrix4()
	{
		*this = Matrix4::Identity;
	}

	/// @brief 
	/// @param values 
	Matrix4::Matrix4(float* values)
	{
		std::memcpy(_values, values, 16 * sizeof(float));
	}

	/// @brief 
	/// @param index 
	/// @return 
	float& Matrix4::operator [] (uint32_t index)
	{
		return _values[index];
	}

	/// @brief 
	/// @param index 
	/// @return 
	float Matrix4::operator [] (uint32_t index) const
	{
		return _values[index];
	}

	/// @brief 
	/// @param right 
	/// @return 
	Matrix4 Matrix4::operator * (const Matrix4 right) const
	{
		Matrix4 result;
		for (uint32_t index = 0; index < 4; ++index)
		{
			result._m1[0] = _m1[0] * right._m1[0] + _m1[1] * right._m2[0] + _m1[2] * right._m3[0] + _m1[3] * right._m4[0];
			result._m1[1] = _m1[0] * right._m1[1] + _m1[1] * right._m2[1] + _m1[2] * right._m3[1] + _m1[3] * right._m4[1];
			result._m1[2] = _m1[0] * right._m1[2] + _m1[1] * right._m2[2] + _m1[2] * right._m3[2] + _m1[3] * right._m4[2];
			result._m1[3] = _m1[0] * right._m1[3] + _m1[1] * right._m2[3] + _m1[2] * right._m3[3] + _m1[3] * right._m4[3];

			result._m2[0] = _m1[0] * right._m2[0] + _m1[1] * right._m2[0] + _m2[2] * right._m3[0] + _m2[3] * right._m4[0];
			result._m2[1] = _m1[0] * right._m2[1] + _m1[1] * right._m2[1] + _m2[2] * right._m3[1] + _m2[3] * right._m4[1];
			result._m2[2] = _m1[0] * right._m2[2] + _m1[1] * right._m2[2] + _m2[2] * right._m3[2] + _m2[3] * right._m4[2];
			result._m2[3] = _m1[0] * right._m2[3] + _m1[1] * right._m2[3] + _m2[2] * right._m3[3] + _m2[3] * right._m4[3];
		}
	}

	/// @brief 
	/// @param right 
	/// @return 
	Matrix4& Matrix4::operator *= (const Matrix4 right)
	{

	}
}
