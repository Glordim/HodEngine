#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/LegacyPadding.hpp"

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(LegacyPadding, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &LegacyPadding::_vector4, "Vector4");
	}

	void LegacyPadding::SetLeft(float left)
	{
		_vector4.SetX(left);
	}

	float LegacyPadding::GetLeft() const
	{
		return _vector4.GetX();
	}

	void LegacyPadding::SetBottom(float bottom)
	{
		_vector4.SetY(bottom);
	}

	float LegacyPadding::GetBottom() const
	{
		return _vector4.GetY();
	}

	void LegacyPadding::SetRight(float right)
	{
		_vector4.SetZ(right);
	}

	float LegacyPadding::GetRight() const
	{
		return _vector4.GetZ();
	}

	void LegacyPadding::SetTop(float top)
	{
		_vector4.SetW(top);
	}

	float LegacyPadding::GetTop() const
	{
		return _vector4.GetW();
	}
}
