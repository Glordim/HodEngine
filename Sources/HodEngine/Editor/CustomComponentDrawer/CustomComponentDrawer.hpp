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
	class HOD_EDITOR_API CustomComponentDrawer
	{
	public:

									CustomComponentDrawer() = default;
									CustomComponentDrawer(const CustomComponentDrawer&) = delete;
									CustomComponentDrawer(CustomComponentDrawer&&) = delete;
			virtual					~CustomComponentDrawer() = default;

			CustomComponentDrawer	operator = (const CustomComponentDrawer&) = delete;
			CustomComponentDrawer	operator = (CustomComponentDrawer&&) = delete;

	public:

		virtual bool				OnDrawInspector(EditorReflectedObject &reflectedObject);
		virtual bool				OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected);
	};
}
