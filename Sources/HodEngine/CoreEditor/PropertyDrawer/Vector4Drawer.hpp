#pragma once
#include "HodEngine/CoreEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline math
{
	class Vector4;
}

namespace hod::inline editor
{
	/// @brief X, Y, Z and W on one line, each mixed on its own in a multi-selection
	class HOD_CORE_EDITOR_API Vector4Drawer : public PropertyDrawer<Vector4>
	{
	protected:
		bool OnDraw(const PropertyValues<Vector4>& values) override;
	};
}
