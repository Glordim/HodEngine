#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include <memory>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	class Component;
}

namespace hod::editor
{
	class ViewportWindow;

	///@brief 
	class HOD_EDITOR_API ComponentCustomEditor
	{
	public:

									ComponentCustomEditor() = default;
									ComponentCustomEditor(const ComponentCustomEditor&) = delete;
									ComponentCustomEditor(ComponentCustomEditor&&) = delete;
			virtual					~ComponentCustomEditor() = default;

			ComponentCustomEditor	operator = (const ComponentCustomEditor&) = delete;
			ComponentCustomEditor	operator = (ComponentCustomEditor&&) = delete;

	public:

		virtual bool				OnDrawInspector(EditorReflectedObject &reflectedObject);
		virtual bool				OnDrawGizmo(std::shared_ptr<game::Component> component, ViewportWindow& viewport);
	};
}
