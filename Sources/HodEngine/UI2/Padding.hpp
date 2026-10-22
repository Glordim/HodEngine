#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/RectOffsets.hpp>

namespace hod::inline ui2
{
	/// @brief Space kept inside a node, around all its children: it pushes them in (see BoxLayout).
	/// Only a RectOffsets typed for what it means, so the editor draws it as such.
	struct HOD_UI2_API Padding : public RectOffsets
	{
		REFLECTED_CLASS_NO_VIRTUAL_WITH_PARENT(Padding, RectOffsets)
	};
}
