#include "HodEngine/Core/Math/Vector2.h"

namespace hod
{
	DESCRIBE_REFLECTED_CLASS(Vector2, void)
	{
		ADD_PROPERTY(Vector2, _x);
		ADD_PROPERTY(Vector2, _y);
	}

	Vector2 Vector2::Zero(0.0f, 0.0f);
	Vector2 Vector2::One(1.0f, 1.0f);

	/// @brief 
	/// @param x 
	/// @param y 
	Vector2::Vector2(float x, float y)
	: _x(x)
	, _y(y)
	{

	}
	
	/// @brief 
	/// @return 
	float Vector2::GetX() const
	{
		return _x;
	}

	/// @brief 
	/// @param x 
	void Vector2::SetX(float x)
	{
		_x = x;
	}

	/// @brief 
	/// @return 
	float Vector2::GetY() const
	{
		return _y;
	}

	/// @brief 
	/// @param y 
	void Vector2::SetY(float y)
	{
		_y = y;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2& Vector2::operator += (const Vector2& right)
	{
		_x += right._x;
		_y += right._y;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2& Vector2::operator -= (const Vector2& right)
	{
		_x -= right._x;
		_y -= right._y;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2& Vector2::operator *= (const Vector2& right)
	{
		_x *= right._x;
		_y *= right._y;
		return *this;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2& Vector2::operator /= (const Vector2& right)
	{
		_x /= right._x;
		_y /= right._y;
		return *this;
	}
	
	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator + (const Vector2& right)
	{
		Vector2 result;
		result._x = _x + right._x;
		result._y = _y + right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator - (const Vector2& right)
	{
		Vector2 result;
		result._x = _x - right._x;
		result._y = _y - right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator * (const Vector2& right)
	{
		Vector2 result;
		result._x = _x * right._x;
		result._y = _y * right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator / (const Vector2& right)
	{
		Vector2 result;
		result._x = _x / right._x;
		result._y = _y / right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	bool Vector2::operator == (const Vector2& right)
	{
		return (_x == right._x && _y == right._y);
	}
}
