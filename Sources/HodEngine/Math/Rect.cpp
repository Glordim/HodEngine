#include "HodEngine/Math/Pch.hpp"
#include "HodEngine/Math/Rect.hpp"
#include "HodEngine/Math/Margin.hpp"

namespace hod::inline math
{
	DESCRIBE_REFLECTED_CLASS(Rect, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Rect::_position, "_position");
		AddPropertyT(reflectionDescriptor, &Rect::_size, "_size");
	}

	/// @brief
	/// @param margin
	/// @return
	Rect Rect::Inset(const Margin& margin) const
	{
		Rect result;
		result._position = _position + Vector2(margin._left, margin._bottom);
		result._size = _size - Vector2(margin._left + margin._right, margin._top + margin._bottom);
		return result;
	}
}
