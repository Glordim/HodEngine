#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
	class Padding;
}

namespace hod::editor
{
	/// @brief 
	class HOD_UI_EDITOR_API PaddingCustomEditor : public CustomPropertyDrawer
	{
	public:

								PaddingCustomEditor() = default;
								PaddingCustomEditor(const PaddingCustomEditor&) = delete;
								PaddingCustomEditor(PaddingCustomEditor&&) = delete;
								~PaddingCustomEditor() override = default;

		PaddingCustomEditor&	operator = (const PaddingCustomEditor&) = delete;
		PaddingCustomEditor&	operator = (PaddingCustomEditor&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}
