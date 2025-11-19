#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Rect.hpp"

namespace hod
{
	DESCRIBE_REFLECTED_CLASS(Rect, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Rect::_position, "_position");
		AddPropertyT(reflectionDescriptor, &Rect::_size, "_size");
	}
}
