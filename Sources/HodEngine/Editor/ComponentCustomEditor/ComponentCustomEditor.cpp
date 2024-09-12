#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

#include "HodEngine/Editor/PropertyDrawer.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param reflectedObject 
	/// @return 
	bool ComponentCustomEditor::OnDrawInspector(EditorReflectedObject &reflectedObject)
	{
		return PropertyDrawer::DrawDescriptor(reflectedObject);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool ComponentCustomEditor::OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue)
	{
		return false;
	}
}
