#pragma once
#include "HodEngine/Math/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline math
{
	struct Margin;

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

		// Shrinks this rect by the given margin (left/right reduce width, top/bottom reduce height).
		Rect Inset(const Margin& margin) const;
	};
}
