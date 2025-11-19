#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Padding.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Padding, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Padding::_vector4, "Vector4");
	}

	void Padding::SetLeft(float left)
	{
		_vector4.SetX(left);
	}

	float Padding::GetLeft() const
	{
		return _vector4.GetX();
	}

	void Padding::SetBottom(float bottom)
	{
		_vector4.SetY(bottom);
	}

	float Padding::GetBottom() const
	{
		return _vector4.GetY();
	}

	void Padding::SetRight(float right)
	{
		_vector4.SetZ(right);
	}

	float Padding::GetRight() const
	{
		return _vector4.GetZ();
	}

	void Padding::SetTop(float top)
	{
		_vector4.SetW(top);
	}

	float Padding::GetTop() const
	{
		return _vector4.GetW();
	}
}
