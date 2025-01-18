#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API WeakComponentCustomEditor : public CustomPropertyDrawer
	{
	public:

									WeakComponentCustomEditor() = default;
									WeakComponentCustomEditor(const WeakComponentCustomEditor&) = delete;
									WeakComponentCustomEditor(WeakComponentCustomEditor&&) = delete;
									~WeakComponentCustomEditor() override = default;

		WeakComponentCustomEditor&	operator = (const WeakComponentCustomEditor&) = delete;
		WeakComponentCustomEditor&	operator = (WeakComponentCustomEditor&&) = delete;

	public:

		bool						Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}
