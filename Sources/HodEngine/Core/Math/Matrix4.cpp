#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"

#include <cstring>
#include <cmath>

namespace hod
{
	Matrix4 Matrix4::Identity;

	/// @brief 
	Matrix4::Matrix4()
	: _values{{1.0f, 0.0f, 0.0f, 0.0f},
			  {0.0f, 1.0f, 0.0f, 0.0f},
			  {0.0f, 0.0f, 1.0f, 0.0f},
			  {0.0f, 0.0f, 0.0f, 1.0f}}
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
	/// @param vec 
	/// @return 
	Vector4 Matrix4::operator * (const Vector4& vec) const
	{
        return Vector4(
            _values[0][0] * vec.GetX() + _values[0][1] * vec.GetY() + _values[0][2] * vec.GetZ() + _values[0][3] * vec.GetW(),
            _values[1][0] * vec.GetX() + _values[1][1] * vec.GetY() + _values[1][2] * vec.GetZ() + _values[1][3] * vec.GetW(),
            _values[2][0] * vec.GetX() + _values[2][1] * vec.GetY() + _values[2][2] * vec.GetZ() + _values[2][3] * vec.GetW(),
            _values[3][0] * vec.GetX() + _values[3][1] * vec.GetY() + _values[3][2] * vec.GetZ() + _values[3][3] * vec.GetW()
        );
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
	/// @return 
	Vector2 Matrix4::GetTranslation() const
	{
		return Vector2(_values[0][3], _values[1][3]);
	}

	/// @brief 
	/// @param translation 
	/// @return 
	Matrix4 Matrix4::Translation(const Vector4& translation)
	{
		Matrix4 result;
		result._values[0][3] = translation.GetX();
		result._values[1][3] = translation.GetY();
		result._values[2][3] = translation.GetZ();
		result._values[3][3] = translation.GetW();
		return result;
	}

	/// @brief 
	/// @param rotation 
	/// @return 
	Matrix4 Matrix4::Rotation(float rotation)
	{
		Matrix4 result;
		result._values[0][0] = std::cos(rotation);
		result._values[1][0] = std::sin(rotation);
		result._values[0][1] = -std::sin(rotation);
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

	/// @brief 
	/// @param matrix 
	/// @return 
	Matrix4 Matrix4::Inverse(const Matrix4& matrix)
	{
		Matrix4 inv;
		float det;
		float invDet;

		inv._values[0][0] = matrix._values[1][1]  * matrix._values[2][2] * matrix._values[3][3] - 
					matrix._values[1][1]  * matrix._values[2][3] * matrix._values[3][2] - 
					matrix._values[2][1]  * matrix._values[1][2]  * matrix._values[3][3] + 
					matrix._values[2][1]  * matrix._values[1][3]  * matrix._values[3][2] +
					matrix._values[3][1] * matrix._values[1][2]  * matrix._values[2][3] - 
					matrix._values[3][1] * matrix._values[1][3]  * matrix._values[2][2];

		inv._values[0][1] = -matrix._values[0][1]  * matrix._values[2][2] * matrix._values[3][3] + 
					matrix._values[0][1]  * matrix._values[2][3] * matrix._values[3][2] + 
					matrix._values[2][1]  * matrix._values[0][2] * matrix._values[3][3] - 
					matrix._values[2][1]  * matrix._values[0][3] * matrix._values[3][2] - 
					matrix._values[3][1] * matrix._values[0][2] * matrix._values[2][3] + 
					matrix._values[3][1] * matrix._values[0][3] * matrix._values[2][2];

		inv._values[0][2] = matrix._values[0][1]  * matrix._values[1][2] * matrix._values[3][3] - 
					matrix._values[0][1]  * matrix._values[1][3] * matrix._values[3][2] - 
					matrix._values[1][1]  * matrix._values[0][2] * matrix._values[3][3] + 
					matrix._values[1][1]  * matrix._values[0][3] * matrix._values[3][2] + 
					matrix._values[3][1] * matrix._values[0][2] * matrix._values[1][3] - 
					matrix._values[3][1] * matrix._values[0][3] * matrix._values[1][2];

		inv._values[0][3] = -matrix._values[0][1]  * matrix._values[1][2] * matrix._values[2][3] + 
					matrix._values[0][1]  * matrix._values[1][3] * matrix._values[2][2] + 
					matrix._values[1][1]  * matrix._values[0][2] * matrix._values[2][3] - 
					matrix._values[1][1]  * matrix._values[0][3] * matrix._values[2][2] - 
					matrix._values[2][1] * matrix._values[0][2] * matrix._values[1][3] + 
					matrix._values[2][1] * matrix._values[0][3] * matrix._values[1][2];

		inv._values[1][0] = -matrix._values[1][0]  * matrix._values[2][2] * matrix._values[3][3] + 
					matrix._values[1][0]  * matrix._values[2][3] * matrix._values[3][2] + 
					matrix._values[2][0]  * matrix._values[1][2] * matrix._values[3][3] - 
					matrix._values[2][0]  * matrix._values[1][3] * matrix._values[3][2] - 
					matrix._values[3][0] * matrix._values[1][2] * matrix._values[2][3] + 
					matrix._values[3][0] * matrix._values[1][3] * matrix._values[2][2];

		inv._values[1][1] = matrix._values[0][0]  * matrix._values[2][2] * matrix._values[3][3] - 
					matrix._values[0][0]  * matrix._values[2][3] * matrix._values[3][2] - 
					matrix._values[2][0]  * matrix._values[0][2] * matrix._values[3][3] + 
					matrix._values[2][0]  * matrix._values[0][3] * matrix._values[3][2] + 
					matrix._values[3][0] * matrix._values[0][2] * matrix._values[2][3] - 
					matrix._values[3][0] * matrix._values[0][3] * matrix._values[2][2];

		inv._values[1][2] = -matrix._values[0][0]  * matrix._values[1][2] * matrix._values[3][3] + 
					matrix._values[0][0]  * matrix._values[1][3] * matrix._values[3][2] + 
					matrix._values[1][0]  * matrix._values[0][2] * matrix._values[3][3] - 
					matrix._values[1][0]  * matrix._values[0][3] * matrix._values[3][2] - 
					matrix._values[3][0] * matrix._values[0][2] * matrix._values[1][3] + 
					matrix._values[3][0] * matrix._values[0][3] * matrix._values[1][2];

		inv._values[1][3] = matrix._values[0][0]  * matrix._values[1][2] * matrix._values[2][3] - 
					matrix._values[0][0]  * matrix._values[1][3] * matrix._values[2][2] - 
					matrix._values[1][0]  * matrix._values[0][2] * matrix._values[2][3] + 
					matrix._values[1][0]  * matrix._values[0][3] * matrix._values[2][2] + 
					matrix._values[2][0] * matrix._values[0][2] * matrix._values[1][3] - 
					matrix._values[2][0] * matrix._values[0][3] * matrix._values[1][2];

		inv._values[2][0] = matrix._values[1][0]  * matrix._values[2][1] * matrix._values[3][3] - 
					matrix._values[1][0]  * matrix._values[2][3] * matrix._values[3][1] - 
					matrix._values[2][0]  * matrix._values[1][1] * matrix._values[3][3] + 
					matrix._values[2][0]  * matrix._values[1][3] * matrix._values[3][1] + 
					matrix._values[3][0] * matrix._values[1][1] * matrix._values[2][3] - 
					matrix._values[3][0] * matrix._values[1][3] * matrix._values[2][1];

		inv._values[2][1] = -matrix._values[0][0]  * matrix._values[2][1] * matrix._values[3][3] + 
					matrix._values[0][0]  * matrix._values[2][3] * matrix._values[3][1] + 
					matrix._values[2][0]  * matrix._values[0][1] * matrix._values[3][3] - 
					matrix._values[2][0]  * matrix._values[0][3] * matrix._values[3][1] - 
					matrix._values[3][0] * matrix._values[0][1] * matrix._values[2][3] + 
					matrix._values[3][0] * matrix._values[0][3] * matrix._values[2][1];

		inv._values[2][2] = matrix._values[0][0]  * matrix._values[1][1] * matrix._values[3][3] - 
					matrix._values[0][0]  * matrix._values[1][3] * matrix._values[3][1] - 
					matrix._values[1][0]  * matrix._values[0][1] * matrix._values[3][3] + 
					matrix._values[1][0]  * matrix._values[0][3] * matrix._values[3][1] + 
					matrix._values[3][0] * matrix._values[0][1] * matrix._values[1][3] - 
					matrix._values[3][0] * matrix._values[0][3] * matrix._values[1][1];

		inv._values[2][3] = -matrix._values[0][0]  * matrix._values[1][1] * matrix._values[2][3] + 
					matrix._values[0][0]  * matrix._values[1][3] * matrix._values[2][1] + 
					matrix._values[1][0]  * matrix._values[0][1] * matrix._values[2][3] - 
					matrix._values[1][0]  * matrix._values[0][3] * matrix._values[2][1] - 
					matrix._values[2][0] * matrix._values[0][1] * matrix._values[1][3] + 
					matrix._values[2][0] * matrix._values[0][3] * matrix._values[1][1];

		inv._values[3][0] = -matrix._values[1][0] * matrix._values[2][1] * matrix._values[3][2] + 
					matrix._values[1][0] * matrix._values[2][2] * matrix._values[3][1] + 
					matrix._values[2][0] * matrix._values[1][1] * matrix._values[3][2] - 
					matrix._values[2][0] * matrix._values[1][2] * matrix._values[3][1] - 
					matrix._values[3][0] * matrix._values[1][1] * matrix._values[2][2] + 
					matrix._values[3][0] * matrix._values[1][2] * matrix._values[2][1];

		inv._values[3][1] = matrix._values[0][0] * matrix._values[2][1] * matrix._values[3][2] - 
					matrix._values[0][0] * matrix._values[2][2] * matrix._values[3][1] - 
					matrix._values[2][0] * matrix._values[0][1] * matrix._values[3][2] + 
					matrix._values[2][0] * matrix._values[0][2] * matrix._values[3][1] + 
					matrix._values[3][0] * matrix._values[0][1] * matrix._values[2][2] - 
					matrix._values[3][0] * matrix._values[0][2] * matrix._values[2][1];

		inv._values[3][2] = -matrix._values[0][0] * matrix._values[1][1] * matrix._values[3][2] + 
					matrix._values[0][0] * matrix._values[1][2] * matrix._values[3][1] + 
					matrix._values[1][0] * matrix._values[0][1] * matrix._values[3][2] - 
					matrix._values[1][0] * matrix._values[0][2] * matrix._values[3][1] - 
					matrix._values[3][0] * matrix._values[0][1] * matrix._values[1][2] + 
					matrix._values[3][0] * matrix._values[0][2] * matrix._values[1][1];

		inv._values[3][3] = matrix._values[0][0] * matrix._values[1][1] * matrix._values[2][2] - 
					matrix._values[0][0] * matrix._values[1][2] * matrix._values[2][1] - 
					matrix._values[1][0] * matrix._values[0][1] * matrix._values[2][2] + 
					matrix._values[1][0] * matrix._values[0][2] * matrix._values[2][1] + 
					matrix._values[2][0] * matrix._values[0][1] * matrix._values[1][2] - 
					matrix._values[2][0] * matrix._values[0][2] * matrix._values[1][1];

		det = matrix._values[0][0] * inv._values[0][0] + matrix._values[0][1] * inv._values[1][0] + matrix._values[0][2] * inv._values[2][0] + matrix._values[0][3] * inv._values[3][0];

		if (det == 0)
		{
			return Matrix4();
		}

		invDet = 1.0f / det;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				inv._values[i][j] *= invDet;
			}
		}

		return inv;
	}

	/// @brief 
	/// @return 
	Quaternion Matrix4::GetRotation() const
	{
		float trace = _values[0][0] + _values[1][1] + _values[2][2];
        float s;
        Quaternion quaternion;

        if (trace > 0)
		{
            s = sqrt(trace + 1.0f) * 2; // 4 * w
            quaternion._w = 0.25f * s;
            quaternion._x = (_values[2][1] - _values[1][2]) / s;
            quaternion._y = (_values[0][2] - _values[2][0]) / s;
            quaternion._z = (_values[1][0] - _values[0][1]) / s;
        }
		else
		{
            if (_values[0][0] > _values[1][1] && _values[0][0] > _values[2][2])
			{
                s = sqrt(1.0f + _values[0][0] - _values[1][1] - _values[2][2]) * 2; // 4 * x
                quaternion._w = (_values[2][1] - _values[1][2]) / s;
                quaternion._x = 0.25f * s;
                quaternion._y = (_values[0][1] + _values[1][0]) / s;
                quaternion._z = (_values[0][2] + _values[2][0]) / s;
            }
			else if (_values[1][1] > _values[2][2])
			{
                s = sqrt(1.0f + _values[1][1] - _values[0][0] - _values[2][2]) * 2; // 4 * y
                quaternion._w = (_values[0][2] - _values[2][0]) / s;
                quaternion._x = (_values[0][1] + _values[1][0]) / s;
                quaternion._y = 0.25f * s;
                quaternion._z = (_values[1][2] + _values[2][1]) / s;
            }
			else
			{
                s = sqrt(1.0f + _values[2][2] - _values[0][0] - _values[1][1]) * 2; // 4 * z
                quaternion._w = (_values[1][0] - _values[0][1]) / s;
                quaternion._x = (_values[0][2] + _values[2][0]) / s;
                quaternion._y = (_values[1][2] + _values[2][1]) / s;
                quaternion._z = 0.25f * s;
            }
        }

        return quaternion;
	}

	/// @brief 
	void Matrix4::RemoveScale()
	{
        float scaleX = std::sqrt(_values[0][0] * _values[0][0] +
                                 _values[1][0] * _values[1][0] +
                                 _values[2][0] * _values[2][0]);

        float scaleY = std::sqrt(_values[0][1] * _values[0][1] +
                                 _values[1][1] * _values[1][1] +
                                 _values[2][1] * _values[2][1]);

        float scaleZ = std::sqrt(_values[0][2] * _values[0][2] +
                                 _values[1][2] * _values[1][2] +
                                 _values[2][2] * _values[2][2]);

        if (scaleX != 0.0f)
		{
            _values[0][0] /= scaleX;
            _values[1][0] /= scaleX;
            _values[2][0] /= scaleX;
        }

        if (scaleY != 0.0f)
		{
            _values[0][1] /= scaleY;
            _values[1][1] /= scaleY;
            _values[2][1] /= scaleY;
        }

        if (scaleZ != 0.0f)
		{
            _values[0][2] /= scaleZ;
            _values[1][2] /= scaleZ;
            _values[2][2] /= scaleZ;
        }
    }
}
