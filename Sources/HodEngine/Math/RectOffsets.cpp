#include "HodEngine/Math/Pch.hpp"
#include "HodEngine/Math/RectOffsets.hpp"

namespace hod::inline math
{
	DESCRIBE_REFLECTED_CLASS(RectOffsets, reflectionDescriptor)
	{
		HOD_PROPERTY(_left);
		HOD_PROPERTY(_top);
		HOD_PROPERTY(_right);
		HOD_PROPERTY(_bottom);
	}
}
