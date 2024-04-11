#pragma once

namespace hod::renderer
{
    enum class FilterMode : uint8_t
	{
		Nearest,
		Linear,

		Count,
	};

	enum class WrapMode : uint8_t
	{
		Clamp,
		Repeat,

		Count,
	};
}
