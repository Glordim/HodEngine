#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CapsuleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp>

namespace hod::editor
{
	/// @brief 
	CapsuleCollider2dComponentCustomEditor::CapsuleCollider2dComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool CapsuleCollider2dComponentCustomEditor::OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue)
	{
		std::shared_ptr<game::CapsuleCollider2dComponent> capsuleCollider2d = std::static_pointer_cast<game::CapsuleCollider2dComponent>(component);
		if (capsuleCollider2d != nullptr)
		{
			std::shared_ptr<game::Node2dComponent> node2D = capsuleCollider2d->GetEntity()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				std::array<Vector2, 65> vertices;
				GeometryGenerator::CapsuleShape<64>(vertices, Vector2::Zero, capsuleCollider2d->GetHeight(), capsuleCollider2d->GetRadius());

				Matrix4 localMatrix = Matrix4::Translation(capsuleCollider2d->GetOffset()) * Matrix4::Rotation(capsuleCollider2d->GetRotation());

				renderer::RenderCommandMesh* renderMeshCommand = new renderer::RenderCommandMesh(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node2D->GetWorldMatrix() * localMatrix, _materialInstance);
				renderQueue.PushRenderCommand(renderMeshCommand);		
			}
		}
		return false;
	}
}
