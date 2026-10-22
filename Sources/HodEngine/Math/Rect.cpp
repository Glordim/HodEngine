#include "HodEngine/Math/Pch.hpp"
#include "HodEngine/Math/Rect.hpp"
#include "HodEngine/Math/RectOffsets.hpp"

namespace hod::inline math
{
	DESCRIBE_REFLECTED_CLASS(Rect, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Rect::_position, "_position");
		AddPropertyT(reflectionDescriptor, &Rect::_size, "_size");
	}

	/// @brief
	/// @param offsets
	/// @return
	Rect Rect::Inset(const RectOffsets& offsets) const
	{
		Rect result;
		result._position = _position + Vector2(offsets._left, offsets._bottom);
		result._size = _size - Vector2(offsets._left + offsets._right, offsets._top + offsets._bottom);
		return result;
	}
}
