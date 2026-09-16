#pragma once
#include "HodEngine/Math/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod::inline math
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	struct HOD_MATH_API Margin
	{
		REFLECTED_CLASS_NO_VIRTUAL(Margin)

	public:
		float _left = 0.0f;
		float _top = 0.0f;
		float _right = 0.0f;
		float _bottom = 0.0f;

		bool operator==(const Margin& right) const = default;
	};
}
