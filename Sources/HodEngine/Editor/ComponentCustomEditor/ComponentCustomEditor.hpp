#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/ImGuizmo.h>
#include <memory>

namespace hod
{
	class ReflectionDescriptor;
	class Matrix4;
}

namespace hod::game
{
	class Component;
}

namespace hod::renderer
{
	class RenderQueue;
}

namespace hod::editor
{
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
		virtual bool				OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue);
	};
}
