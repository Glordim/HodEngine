#include "HodEngine/Math/Pch.hpp"
#include "HodEngine/Math/Rect.hpp"

namespace hod::math
{
	DESCRIBE_REFLECTED_CLASS(Rect, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Rect::_position, "_position");
		AddPropertyT(reflectionDescriptor, &Rect::_size, "_size");
	}
}
