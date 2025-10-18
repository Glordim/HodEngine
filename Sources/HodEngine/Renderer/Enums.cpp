#include "HodEngine/Renderer/Enums.hpp"
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::renderer
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
