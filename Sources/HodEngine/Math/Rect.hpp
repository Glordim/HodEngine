#pragma once
#include "HodEngine/Math/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline math
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	struct HOD_MATH_API Rect
	{
		REFLECTED_CLASS_NO_VIRTUAL(Rect)

	public:
		Vector2 _position;
		Vector2 _size;

		bool operator==(const Rect& right) const = default;
	};
}
