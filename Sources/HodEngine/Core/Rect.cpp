#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Rect.hpp"

namespace hod
{
	DESCRIBE_REFLECTED_CLASS_NO_PARENT(Rect)
	{
		ADD_PROPERTY(Rect, _position);
		ADD_PROPERTY(Rect, _size);
	}
}
