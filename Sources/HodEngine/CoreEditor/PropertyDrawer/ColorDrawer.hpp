#pragma once
#include "HodEngine/CoreEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline math
{
	struct Color;
}

namespace hod::inline editor
{
	/// @brief Its color field (a swatch opening the picker)
	class HOD_CORE_EDITOR_API ColorDrawer : public PropertyDrawer<Color>
	{
	protected:
		bool OnDraw(const PropertyValues<Color>& values) override;
	};
}
