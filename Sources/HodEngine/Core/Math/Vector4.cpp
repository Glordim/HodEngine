#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"

namespace hod
{
	DESCRIBE_REFLECTED_CLASS(Vector4, void)
	{
		ADD_PROPERTY(Vector4, _x);
		ADD_PROPERTY(Vector4, _y);
		ADD_PROPERTY(Vector4, _z);
		ADD_PROPERTY(Vector4, _w);
	}

	Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 Vector4::One(1.0f, 1.0f, 1.0f, 1.0f);

	/// @brief 
	/// @param x 
	/// @param y 
	/// @param z 
	/// @param w 
	Vector4::Vector4(float x, float y, float z, float w)
	: _x(x)
	, _y(y)
	, _z(z)
	, _w(w)
	{

	}
	
	/// @brief 
	/// @return 
	float Vector4::GetX() const
	{
		return _x;
	}

	/// @brief 
	/// @param x 
	void Vector4::SetX(float x)
	{
		_x = x;
	}

	/// @brief 
	/// @return 
	float Vector4::GetY() const
	{
		return _y;
	}

	/// @brief 
	/// @param y 
	void Vector4::SetY(float y)
	{
		_y = y;
	}

	/// @brief 
	/// @return 
	float Vector4::GetZ() const
	{
		return _z;
	}

	/// @brief 
	/// @param z 
	void Vector4::SetZ(float z)
	{
		_z = z;
	}

	/// @brief 
	/// @return 
	float Vector4::GetW() const
	{
		return _w;
	}

	/// @brief 
	/// @param w 
	void Vector4::SetW(float w)
	{
		_w = w;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator += (const Vector4& right)
	{
		_x += right._x;
		_y += right._y;
		_z += right._z;
		_w += right._w;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator -= (const Vector4& right)
	{
		_x -= right._x;
		_y -= right._y;
		_z -= right._z;
		_w -= right._w;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator *= (const Vector4& right)
	{
		_x *= right._x;
		_y *= right._y;
		_z *= right._z;
		_w *= right._w;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator /= (const Vector4& right)
	{
		_x /= right._x;
		_y /= right._y;
		_z /= right._z;
		_w /= right._w;
		return *this;
	}
	
	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator + (const Vector4& right) const
	{
		Vector4 result;
		result._x = _x + right._x;
		result._y = _y + right._y;
		result._z = _z + right._z;
		result._w = _w + right._w;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator - (const Vector4& right) const
	{
		Vector4 result;
		result._x = _x - right._x;
		result._y = _y - right._y;
		result._z = _z - right._z;
		result._w = _w - right._w;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator * (const Vector4& right) const
	{
		Vector4 result;
		result._x = _x * right._x;
		result._y = _y * right._y;
		result._z = _z * right._z;
		result._w = _w * right._w;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator / (const Vector4& right) const
	{
		Vector4 result;
		result._x = _x / right._x;
		result._y = _y / right._y;
		result._z = _z / right._z;
		result._w = _w / right._w;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	bool Vector4::operator == (const Vector4& right) const
	{
		return (_x == right._x && _y == right._y && _z == right._z && _w == right._w);
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator + (float value) const
	{
		Vector4 result;
		result._x = _x + value;
		result._y = _y + value;
		result._z = _z + value;
		result._w = _w + value;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator - (float value) const
	{
		Vector4 result;
		result._x = _x - value;
		result._y = _y - value;
		result._z = _z - value;
		result._w = _w - value;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator * (float value) const
	{
		Vector4 result;
		result._x = _x * value;
		result._y = _y * value;
		result._z = _z * value;
		result._w = _w * value;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4 Vector4::operator / (float value) const
	{
		Vector4 result;
		result._x = _x / value;
		result._y = _y / value;
		result._z = _z / value;
		result._w = _w / value;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator += (float value)
	{
		_x += value;
		_y += value;
		_z += value;
		_w += value;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator -= (float value)
	{
		_x -= value;
		_y -= value;
		_z -= value;
		_w -= value;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator *= (float value)
	{
		_x *= value;
		_y *= value;
		_z *= value;
		_w *= value;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector4& Vector4::operator /= (float value)
	{
		_x /= value;
		_y /= value;
		_z /= value;
		_w /= value;
		return *this;
	}

	/// @brief 
	/// @param vector 
	/// @return 
	Vector4 Vector4::Normalize(const Vector4& vector)
	{
		float magnitude = std::sqrt(vector._x * vector._x + 
                                vector._y * vector._y + 
                                vector._z * vector._z + 
                                vector._w * vector._w);

		if (magnitude == 0.0f)
		{
			return Vector4::Zero;
		}

		return vector / magnitude;
	}

	/// @brief 
	/// @param vec1 
	/// @param vec2 
	/// @return 
	float Vector4::Dot(const Vector4& vec1, const Vector4& vec2)
	{
    	return (vec1._x * vec2._x) +
           (vec1._y * vec2._y) +
           (vec1._z * vec2._z) +
           (vec1._w * vec2._w);
	}
}
