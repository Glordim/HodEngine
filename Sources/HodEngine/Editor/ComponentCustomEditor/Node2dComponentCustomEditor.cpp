#include "HodEngine/Editor/Pch.hpp"
#include "Node2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/ImGui/DearImGui/ImGuizmo.h>

namespace hod::editor
{
	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool Node2dComponentCustomEditor::OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue)
	{
		std::shared_ptr<game::Node2dComponent> node2D = std::static_pointer_cast<game::Node2dComponent>(component);
		if (node2D != nullptr)
		{
			float matrix[16];
			float position[3];
			position[0] = node2D->GetPosition().GetX();
			position[1] = node2D->GetPosition().GetY();
			position[2] = 0.0f;
			float rotation[3];
			rotation[0] = 0.0f;
			rotation[1] = 0.0f;
			rotation[2] = node2D->GetRotation();
			float scale[3];
			scale[0] = node2D->GetScale().GetX();
			scale[1] = node2D->GetScale().GetY();
			scale[2] = 1.0f;
			ImGuizmo::RecomposeMatrixFromComponents(position, rotation, scale, matrix);

			if (ImGuizmo::Manipulate((float*)&view, (float*)&projection, operation, ImGuizmo::MODE::LOCAL, matrix))
			{
				ImGuizmo::DecomposeMatrixToComponents(matrix, position, rotation, scale);
				node2D->SetPosition(Vector2(position[0], position[1]));
				node2D->SetRotation(rotation[2]);
				node2D->SetScale(Vector2(scale[0], scale[1]));
				return true;
			}
		}

		return false;
	}
}
