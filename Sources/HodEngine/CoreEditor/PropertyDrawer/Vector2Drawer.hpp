#pragma once
#include "HodEngine/CoreEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline math
{
	class Vector2;
}

namespace hod::inline editor
{
	/// @brief X and Y on one line, each mixed on its own in a multi-selection
	class HOD_CORE_EDITOR_API Vector2Drawer : public PropertyDrawer<Vector2>
	{
	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;
	};
}
