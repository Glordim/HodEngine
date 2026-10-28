#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/CornerRadii.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(CornerRadii, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CornerRadii::_topLeft, "TopLeft");
		AddPropertyT(reflectionDescriptor, &CornerRadii::_topRight, "TopRight");
		AddPropertyT(reflectionDescriptor, &CornerRadii::_bottomRight, "BottomRight");
		AddPropertyT(reflectionDescriptor, &CornerRadii::_bottomLeft, "BottomLeft");
	}
}
