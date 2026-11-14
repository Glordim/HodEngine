#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/CornerRadii.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(CornerRadii)
	{
		HOD_PROPERTY(_topLeft);
		HOD_PROPERTY(_topRight);
		HOD_PROPERTY(_bottomRight);
		HOD_PROPERTY(_bottomLeft);
	}
}
