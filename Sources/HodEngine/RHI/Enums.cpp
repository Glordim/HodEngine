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

	DESCRIBE_REFLECTED_ENUM(BlendMode, enumReflection)
	{
		enumReflection.AddEnumValue(BlendMode::Opaque, "Opaque");
		enumReflection.AddEnumValue(BlendMode::Alpha, "Alpha");
		enumReflection.AddEnumValue(BlendMode::PremultipliedAlpha, "PremultipliedAlpha");
		enumReflection.AddEnumValue(BlendMode::Additive, "Additive");
	}

	DESCRIBE_REFLECTED_ENUM(CullMode, enumReflection)
	{
		enumReflection.AddEnumValue(CullMode::None, "None");
		enumReflection.AddEnumValue(CullMode::Back, "Back");
		enumReflection.AddEnumValue(CullMode::Front, "Front");
	}
}
