#include "HodEngine/Math/Pch.hpp"
#include "HodEngine/Math/Margin.hpp"

namespace hod::inline math
{
	DESCRIBE_REFLECTED_CLASS(Margin, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Margin::_left, "_left");
		AddPropertyT(reflectionDescriptor, &Margin::_top, "_top");
		AddPropertyT(reflectionDescriptor, &Margin::_right, "_right");
		AddPropertyT(reflectionDescriptor, &Margin::_bottom, "_bottom");
	}
}
