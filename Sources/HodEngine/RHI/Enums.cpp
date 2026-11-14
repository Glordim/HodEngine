#include "HodEngine/RHI/Enums.hpp"
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline rhi
{
	DESCRIBE_REFLECTED_ENUM(FilterMode)
	{
		HOD_ENUM_VALUE(Linear);
		HOD_ENUM_VALUE(Nearest);
	}

	DESCRIBE_REFLECTED_ENUM(WrapMode)
	{
		HOD_ENUM_VALUE(Clamp);
		HOD_ENUM_VALUE(Repeat);
	}

	DESCRIBE_REFLECTED_ENUM(BlendMode)
	{
		HOD_ENUM_VALUE(Opaque);
		HOD_ENUM_VALUE(Alpha);
		HOD_ENUM_VALUE(PremultipliedAlpha);
		HOD_ENUM_VALUE(Additive);
	}

	DESCRIBE_REFLECTED_ENUM(CullMode)
	{
		HOD_ENUM_VALUE(None);
		HOD_ENUM_VALUE(Back);
		HOD_ENUM_VALUE(Front);
	}
}
