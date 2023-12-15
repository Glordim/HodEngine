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
	Vector4 Vector4::operator + (const Vector4& right)
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
	Vector4 Vector4::operator - (const Vector4& right)
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
	Vector4 Vector4::operator * (const Vector4& right)
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
	Vector4 Vector4::operator / (const Vector4& right)
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
	bool Vector4::operator == (const Vector4& right)
	{
		return (_x == right._x && _y == right._y && _z == right._z && _w == right._w);
	}
}
