#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

#include <cmath>

namespace hod
{
	DESCRIBE_REFLECTED_CLASS(Vector2, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Vector2::_x, "_x");
		AddPropertyT(reflectionDescriptor, &Vector2::_y, "_y");
	}

	Vector2 Vector2::Zero(0.0f, 0.0f);
	Vector2 Vector2::One(1.0f, 1.0f);
	Vector2 Vector2::Up(0.0f, 1.0f);
	Vector2 Vector2::Down(0.0f, -1.0f);
	Vector2 Vector2::Left(-1.0f, 0.0f);
	Vector2 Vector2::Right(1.0f, 0.0f);

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
	/// @return 
	Vector2 Vector2::operator-() const
	{
		return Vector2(-_x, -_y);
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
	Vector2 Vector2::operator + (const Vector2& right) const
	{
		Vector2 result;
		result._x = _x + right._x;
		result._y = _y + right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator - (const Vector2& right) const
	{
		Vector2 result;
		result._x = _x - right._x;
		result._y = _y - right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator * (const Vector2& right) const
	{
		Vector2 result;
		result._x = _x * right._x;
		result._y = _y * right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	Vector2 Vector2::operator / (const Vector2& right) const
	{
		Vector2 result;
		result._x = _x / right._x;
		result._y = _y / right._y;
		return result;
	}

	/// @brief 
	/// @param right 
	/// @return 
	bool Vector2::operator == (const Vector2& right) const
	{
		return (_x == right._x && _y == right._y);
	}

	Vector2& Vector2::operator += (float right)
	{
		return operator += (Vector2(right, right));
	}

	Vector2& Vector2::operator -= (float right)
	{
		return operator -= (Vector2(right, right));
	}

	Vector2& Vector2::operator *= (float right)
	{
		return operator *= (Vector2(right, right));
	}
	
	Vector2& Vector2::operator /= (float right)
	{
		return operator /= (Vector2(right, right));
	}

	Vector2 Vector2::operator + (float right) const
	{
		return operator + (Vector2(right, right));
	}

	Vector2 Vector2::operator - (float right) const
	{
		return operator - (Vector2(right, right));
	}
	
	Vector2 Vector2::operator * (float right) const
	{
		return operator * (Vector2(right, right));
	}

	Vector2 Vector2::operator / (float right) const
	{
		return operator / (Vector2(right, right));
	}

	/// @brief 
	/// @param vector 
	/// @return 
	Vector2 Vector2::Normalize(const Vector2& vector)
	{
		float magnitude = std::sqrt(vector._x * vector._x + 
                                vector._y * vector._y);

		if (magnitude == 0.0f)
		{
			return Vector2::Zero;
		}

		return vector / magnitude;
	}

	/// @brief 
	/// @param A 
	/// @param B 
	/// @return 
	float Vector2::Dot(const Vector2& A, const Vector2& B)
	{
        return A._x * B._x + A._y * B._y;
    }

	/// @brief 
	/// @param velocity 
	/// @param normal 
	/// @return 
	Vector2 Vector2::Reflect(const Vector2& velocity, const Vector2& normal)
	{
        float dotProduct = Vector2::Dot(velocity, normal);
		return velocity - (normal * dotProduct * 2);
    }

	/// @brief 
	/// @param A 
	/// @param B 
	/// @param t 
	/// @return 
	Vector2 Vector2::Lerp(const Vector2& A, const Vector2& B, float t)
	{
		return Vector2(std::lerp(A._x, B._x, t), std::lerp(A._y, B._y, t));
	}

	/// @brief 
	/// @param A 
	/// @param B 
	/// @return 
	float Vector2::Distance(const Vector2& A, const Vector2& B)
	{
		return std::sqrt((B._x - A._x) * (B._x - A._x) + 
						(B._y - A._y) * (B._y - A._y));
	}

	/// @brief 
	/// @param A 
	/// @param B 
	/// @return 
	float Vector2::Angle(const Vector2& A, const Vector2& B)
	{
		return std::atan2(B._y - A._y, B._x - A._x);
	}
}
