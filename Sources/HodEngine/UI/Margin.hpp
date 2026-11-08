#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Math/RectOffsets.hpp>

namespace hod::inline ui
{
	/// @brief Space kept around a child, outside its rect: it pushes its neighbours away (see BoxLayoutParams).
	/// Only a RectOffsets typed for what it means, so the editor draws it as such.
	struct HOD_UI_API Margin : public RectOffsets
	{
		REFLECTED_CLASS_NO_VIRTUAL_WITH_PARENT(Margin, RectOffsets)
	};
}
