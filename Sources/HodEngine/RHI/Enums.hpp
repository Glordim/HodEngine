#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline rhi
{
	enum class FilterMode : uint8_t
	{
		Nearest,
		Linear,

		Count,
	};
	REFLECTED_ENUM2(HOD_RHI_API, FilterMode);

	enum class WrapMode : uint8_t
	{
		Clamp,
		Repeat,

		Count,
	};
	REFLECTED_ENUM2(HOD_RHI_API, WrapMode);

}
