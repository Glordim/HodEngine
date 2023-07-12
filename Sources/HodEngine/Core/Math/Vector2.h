#pragma once

#include "HodEngine/Core/Reflection/ReflectionMacros.h"

namespace hod
{
	class Vector2 final
	{
		REFLECTED_CLASS_NO_PARENT(Vector2)

	public:

		static Vector2 Zero;
		static Vector2 One;

	public:

					Vector2() = default;
					Vector2(float x, float y);
					Vector2(const Vector2& copy) = default;
					Vector2(Vector2&& move) = default;
					~Vector2() = default;
		
		Vector2&	operator = (const Vector2& copy) = default;
		Vector2&	operator = (Vector2&& move) = default;

	public:

		Vector2&	operator += (const Vector2& right);
		Vector2&	operator -= (const Vector2& right);
		Vector2&	operator *= (const Vector2& right);
		Vector2&	operator /= (const Vector2& right);

		Vector2		operator + (const Vector2& right);
		Vector2		operator - (const Vector2& right);
		Vector2		operator * (const Vector2& right);
		Vector2		operator / (const Vector2& right);

		bool		operator == (const Vector2& right);

	public:

		float		GetX() const;
		void		SetX(float x);

		float		GetY() const;
		void		SetY(float y);

	private:

		float		_x = 0.0f;
		float		_y = 0.0f;
	};
}
