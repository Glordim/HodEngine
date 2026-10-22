#pragma once
#include "HodEngine/Math/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod::inline math
{
	//-----------------------------------------------------------------------------
	//! @brief A distance from each edge of a rect (Y-up: bottom is the low edge). No direction of its own:
	//! what it means (shrink, grow, keep away...) is up to whoever uses it, e.g. Rect::Inset.
	//-----------------------------------------------------------------------------
	struct HOD_MATH_API RectOffsets
	{
		REFLECTED_CLASS_NO_VIRTUAL(RectOffsets)

	public:
		float _left = 0.0f;
		float _top = 0.0f;
		float _right = 0.0f;
		float _bottom = 0.0f;

		bool operator==(const RectOffsets& right) const = default;
	};
}
