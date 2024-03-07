#pragma once

#include <cstdint>

namespace hod
{
	class Vector2;
	class Vector4;
	
	class Matrix4 final
	{
	public:

		static Matrix4 Identity;

	public:

					Matrix4();
					Matrix4(float* values);
					Matrix4(const Matrix4& copy) = default;
					Matrix4(Matrix4&& move) = default;
					~Matrix4() = default;

		Matrix4&	operator = (const Matrix4& copy) = default;
		Matrix4&	operator = (Matrix4&& move) = default;

	public:

		float&		operator [] (uint32_t index);
		float		operator [] (uint32_t index) const;

		Matrix4		operator * (const Matrix4& right) const;

		Matrix4&	operator *= (const Matrix4& right);

	public:

		static Matrix4	OrthogonalProjection(float left, float right, float bottom, float top, float near, float far);
		static Matrix4	Translation(const Vector2& translation);
		static Matrix4	Translation(const Vector4& translation);
		static Matrix4	Rotation(float rotation);
		static Matrix4	Scale(const Vector2& scale);

	private:

		float 		_values[4][4];
	};
}
