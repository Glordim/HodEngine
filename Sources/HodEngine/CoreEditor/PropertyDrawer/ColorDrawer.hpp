#pragma once
#include "HodEngine/CoreEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline math
{
	struct Color;
}

namespace hod::inline editor
{
	/// @brief
	class HOD_CORE_EDITOR_API ColorDrawer : public PropertyDrawer<Color>
	{
	public:
		static bool Draw(Color& value);

	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;
	};
}
