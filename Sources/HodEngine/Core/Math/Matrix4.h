#pragma once

#include <cstdint>

namespace hod
{
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

		Matrix4		operator * (const Matrix4 right) const;

		Matrix4&	operator *= (const Matrix4 right);

	private:

		float 		_m1[4];
		float 		_m2[4];
		float 		_m3[4];
		float 		_m4[4];
	};
}
