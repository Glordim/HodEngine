#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline core
{
	class ReflectionPropertyObject;
}

namespace hod::inline math
{
	struct Color;
}

namespace hod::editor
{
	/// @brief
	class HOD_CORE_EDITOR_API ColorCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:
		ColorCustomPropertyDrawer() = default;
		ColorCustomPropertyDrawer(const ColorCustomPropertyDrawer&) = delete;
		ColorCustomPropertyDrawer(ColorCustomPropertyDrawer&&) = delete;
		~ColorCustomPropertyDrawer() override = default;

		ColorCustomPropertyDrawer& operator=(const ColorCustomPropertyDrawer&) = delete;
		ColorCustomPropertyDrawer& operator=(ColorCustomPropertyDrawer&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool Draw(Color& value);
	};
}
