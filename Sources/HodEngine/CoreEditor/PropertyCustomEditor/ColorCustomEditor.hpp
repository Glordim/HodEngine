#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
	struct Color;
}

namespace hod::editor
{
	/// @brief 
	class HOD_CORE_EDITOR_API ColorCustomEditor : public CustomPropertyDrawer
	{
	public:

								ColorCustomEditor() = default;
								ColorCustomEditor(const ColorCustomEditor&) = delete;
								ColorCustomEditor(ColorCustomEditor&&) = delete;
								~ColorCustomEditor() override = default;

		ColorCustomEditor&		operator = (const ColorCustomEditor&) = delete;
		ColorCustomEditor&		operator = (ColorCustomEditor&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool				Draw(Color& value);
	};
}
