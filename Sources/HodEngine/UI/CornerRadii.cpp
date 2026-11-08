#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/CornerRadii.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(CornerRadii, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CornerRadii::_topLeft, "TopLeft");
		AddPropertyT(reflectionDescriptor, &CornerRadii::_topRight, "TopRight");
		AddPropertyT(reflectionDescriptor, &CornerRadii::_bottomRight, "BottomRight");
		AddPropertyT(reflectionDescriptor, &CornerRadii::_bottomLeft, "BottomLeft");
	}
}
