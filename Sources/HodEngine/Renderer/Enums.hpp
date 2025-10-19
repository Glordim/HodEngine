#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::renderer
{
	enum class FilterMode : uint8_t
	{
		Nearest,
		Linear,

		Count,
	};
	REFLECTED_ENUM2(HOD_RENDERER_API, FilterMode);

	enum class WrapMode : uint8_t
	{
		Clamp,
		Repeat,

		Count,
	};
	REFLECTED_ENUM2(HOD_RENDERER_API, WrapMode);

}
