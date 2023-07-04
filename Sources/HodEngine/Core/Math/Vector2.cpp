#include "HodEngine/Core/Math/Vector2.h"

namespace hod
{
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
}