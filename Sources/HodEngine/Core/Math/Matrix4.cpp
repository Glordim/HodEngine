#include "HodEngine/Core/Math/Matrix4.h"
#include "HodEngine/Core/Math/Vector2.h"

#include <cstring>

namespace hod
{
	Matrix4 Matrix4::Identity;

	/// @brief 
	Matrix4::Matrix4()
	: _values{1.0f, 0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f, 0.0f,
			  0.0f, 0.0f, 1.0f, 0.0f,
			  0.0f, 0.0f, 0.0f, 1.0f}
	{
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
		return _values[index / 4][index % 4];
	}

	/// @brief 
	/// @param index 
	/// @return 
	float Matrix4::operator [] (uint32_t index) const
	{
		return _values[index / 4][index % 4];
	}

	/// @brief 
	/// @param right 
	/// @return 
	Matrix4 Matrix4::operator * (const Matrix4& right) const
	{
		Matrix4 result;
		for (uint32_t index = 0; index < 4; ++index)
		{
			result._values[index][0] = (_values[index][0] * right._values[0][0]) + (_values[index][1] * right._values[1][0]) + (_values[index][2] * right._values[2][0]) + (_values[index][3] * right._values[3][0]);
			result._values[index][1] = (_values[index][0] * right._values[0][1]) + (_values[index][1] * right._values[1][1]) + (_values[index][2] * right._values[2][1]) + (_values[index][3] * right._values[3][1]);
			result._values[index][2] = (_values[index][0] * right._values[0][2]) + (_values[index][1] * right._values[1][2]) + (_values[index][2] * right._values[2][2]) + (_values[index][3] * right._values[3][2]);
			result._values[index][3] = (_values[index][0] * right._values[0][3]) + (_values[index][1] * right._values[1][3]) + (_values[index][2] * right._values[2][3]) + (_values[index][3] * right._values[3][3]);
		}
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Matrix4& Matrix4::operator *= (const Matrix4& right)
	{
		*this = *this * right;
		return *this;
	}

	/// @brief 
	/// @param left 
	/// @param right 
	/// @param bottom 
	/// @param top 
	/// @param near 
	/// @param top 
	/// @return 
	Matrix4 Matrix4::OrthogonalProjection(float left, float right, float bottom, float top, float near, float far)
	{
		Matrix4 projection;

		projection._values[0][0] = 2.0f / (right - left);
		projection._values[0][1] = 0.0f;
		projection._values[0][2] = 0.0f;
		projection._values[0][3] = 0.0f;

		projection._values[1][0] = 0.0f;
		projection._values[1][1] = 2.0f / (top - bottom);
		projection._values[1][2] = 0.0f;
		projection._values[1][3] = 0.0f;

		projection._values[2][0] = 0.0f;
		projection._values[2][1] = 0.0f;
		projection._values[2][2] = 1.0f;
		projection._values[2][3] = 0.0f;

		projection._values[3][0] = 0.0f;
		projection._values[3][1] = 0.0f;
		projection._values[3][2] = 0.0f;
		projection._values[3][3] = 1.0f;

		return projection;
	}

	/// @brief 
	/// @param translation 
	/// @return 
	Matrix4 Matrix4::Translation(const Vector2& translation)
	{
		Matrix4 result;
		result._values[0][3] = translation.GetX();
		result._values[1][3] = translation.GetY();
		return result;
	}

	/// @brief 
	/// @param rotation 
	/// @return 
	Matrix4 Matrix4::Rotation(float rotation)
	{
		Matrix4 result;
		result._values[0][0] = std::cos(rotation);
		result._values[0][1] = -std::sin(rotation);
		result._values[1][0] = std::sin(rotation);
		result._values[1][1] = std::cos(rotation);
		return result;
	}

	/// @brief 
	/// @param scale 
	/// @return 
	Matrix4 Matrix4::Scale(const Vector2& scale)
	{
		Matrix4 result;
		result._values[0][0] = scale.GetX();
		result._values[1][1] = scale.GetY();
		return result;
	}
}
