#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
	class WeakResourceBase;
}

namespace hod::editor
{
	/// @brief 
	class HOD_CORE_EDITOR_API WeakResourceCustomEditor : public CustomPropertyDrawer
	{
	public:

									WeakResourceCustomEditor() = default;
									WeakResourceCustomEditor(const WeakResourceCustomEditor&) = delete;
									WeakResourceCustomEditor(WeakResourceCustomEditor&&) = delete;
									~WeakResourceCustomEditor() override = default;

		WeakResourceCustomEditor&	operator = (const WeakResourceCustomEditor&) = delete;
		WeakResourceCustomEditor&	operator = (WeakResourceCustomEditor&&) = delete;

	public:

		bool						Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool					Draw(WeakResourceBase& weakResource);
	};
}
