#include "HodEngine/Math/Pch.hpp"
#include "HodEngine/Math/RectOffsets.hpp"

namespace hod::inline math
{
	DESCRIBE_REFLECTED_CLASS(RectOffsets, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &RectOffsets::_left, "_left");
		AddPropertyT(reflectionDescriptor, &RectOffsets::_top, "_top");
		AddPropertyT(reflectionDescriptor, &RectOffsets::_right, "_right");
		AddPropertyT(reflectionDescriptor, &RectOffsets::_bottom, "_bottom");
	}
}
