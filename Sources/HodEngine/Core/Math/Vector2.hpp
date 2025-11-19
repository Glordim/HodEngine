#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	class HOD_CORE_API Vector2 final
	{
		REFLECTED_CLASS_NO_VIRTUAL(Vector2)

	public:
		static Vector2 Zero;
		static Vector2 One;
		static Vector2 Up;
		static Vector2 Down;
		static Vector2 Left;
		static Vector2 Right;

	public:
		Vector2() = default;
		Vector2(float x, float y);
		Vector2(const Vector2& copy) = default;
		Vector2(Vector2&& move) = default;
		~Vector2() = default;

		Vector2& operator=(const Vector2& copy) = default;
		Vector2& operator=(Vector2&& move) = default;

	public:
		Vector2 operator-() const;

		Vector2& operator+=(const Vector2& right);
		Vector2& operator-=(const Vector2& right);
		Vector2& operator*=(const Vector2& right);
		Vector2& operator/=(const Vector2& right);

		Vector2 operator+(const Vector2& right) const;
		Vector2 operator-(const Vector2& right) const;
		Vector2 operator*(const Vector2& right) const;
		Vector2 operator/(const Vector2& right) const;

		bool operator==(const Vector2& right) const;

		Vector2& operator+=(float right);
		Vector2& operator-=(float right);
		Vector2& operator*=(float right);
		Vector2& operator/=(float right);

		Vector2 operator+(float right) const;
		Vector2 operator-(float right) const;
		Vector2 operator*(float right) const;
		Vector2 operator/(float right) const;

	public:
		static Vector2 Normalize(const Vector2& vector);
		static float   Dot(const Vector2& A, const Vector2& B);
		static Vector2 Reflect(const Vector2& velocity, const Vector2& normal);
		static float   Angle(const Vector2& A, const Vector2& B);

		static Vector2 Lerp(const Vector2& A, const Vector2& B, float t);
		static float   Distance(const Vector2& A, const Vector2& B);

	public:
		float GetX() const;
		void  SetX(float x);

		float GetY() const;
		void  SetY(float y);

	private:
		float _x = 0.0f;
		float _y = 0.0f;
	};
}
