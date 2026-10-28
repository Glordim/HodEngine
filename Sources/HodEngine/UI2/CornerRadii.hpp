#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline ui2
{
	/// @brief How much each corner of a rect is rounded (see ShapeNode). A radius larger than half of the
	/// rect's shortest side is clamped to it when drawn: a large value makes a pill.
	struct HOD_UI2_API CornerRadii
	{
		REFLECTED_CLASS_NO_VIRTUAL(CornerRadii)

	public:
		float _topLeft = 0.0f;
		float _topRight = 0.0f;
		float _bottomRight = 0.0f;
		float _bottomLeft = 0.0f;

		bool operator==(const CornerRadii& right) const = default;
	};
}
