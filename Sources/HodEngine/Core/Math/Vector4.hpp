#pragma once

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	class Vector4 final
	{
		REFLECTED_CLASS_NO_VIRTUAL(Vector4)

	public:

		static Vector4 Zero;
		static Vector4 One;

	public:

					Vector4() = default;
					Vector4(float x, float y, float z, float w);
					Vector4(const Vector4& copy) = default;
					Vector4(Vector4&& move) = default;
					~Vector4() = default;
		
		Vector4&	operator = (const Vector4& copy) = default;
		Vector4&	operator = (Vector4&& move) = default;

	public:

		Vector4&	operator += (const Vector4& right);
		Vector4&	operator -= (const Vector4& right);
		Vector4&	operator *= (const Vector4& right);
		Vector4&	operator /= (const Vector4& right);

		Vector4		operator + (const Vector4& right) const;
		Vector4		operator - (const Vector4& right) const;
		Vector4		operator * (const Vector4& right) const;
		Vector4		operator / (const Vector4& right) const;

		bool		operator == (const Vector4& right) const;

		Vector4&	operator += (float value);
		Vector4&	operator -= (float value);
		Vector4&	operator *= (float value);
		Vector4&	operator /= (float value);

		Vector4		operator + (float value) const;
		Vector4		operator - (float value) const;
		Vector4		operator * (float value) const;
		Vector4		operator / (float value) const;

	public:

		float		GetX() const;
		void		SetX(float x);

		float		GetY() const;
		void		SetY(float y);

		float		GetZ() const;
		void		SetZ(float z);

		float		GetW() const;
		void		SetW(float w);

	private:

		float		_x = 0.0f;
		float		_y = 0.0f;
		float		_z = 0.0f;
		float		_w = 0.0f;
	};
}
