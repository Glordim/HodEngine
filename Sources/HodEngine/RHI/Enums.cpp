#include "HodEngine/RHI/Enums.hpp"
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline rhi
{
	DESCRIBE_REFLECTED_ENUM(FilterMode, enumReflection)
	{
		enumReflection.AddEnumValue(FilterMode::Linear, "Linear");
		enumReflection.AddEnumValue(FilterMode::Nearest, "Nearest");
	}

	DESCRIBE_REFLECTED_ENUM(WrapMode, enumReflection)
	{
		enumReflection.AddEnumValue(WrapMode::Clamp, "Clamp");
		enumReflection.AddEnumValue(WrapMode::Repeat, "Repeat");
	}
}
