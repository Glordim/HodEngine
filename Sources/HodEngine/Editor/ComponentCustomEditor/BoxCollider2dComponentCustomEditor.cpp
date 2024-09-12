#include "HodEngine/Editor/Pch.hpp"
#include "BoxCollider2dComponentCustomEditor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp>

namespace hod::editor
{
	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool BoxCollider2dComponentCustomEditor::OnDrawGizmo(std::shared_ptr<game::Component> component, const Matrix4& projection, const Matrix4& view, ImGuizmo::OPERATION operation, renderer::RenderQueue& renderQueue)
	{
		std::shared_ptr<game::BoxCollider2dComponent> boxCollider2d = std::static_pointer_cast<game::BoxCollider2dComponent>(component);
		if (boxCollider2d != nullptr)
		{
			std::shared_ptr<game::Node2dComponent> node2D = boxCollider2d->GetEntity()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				if (_materialInstance == nullptr)
				{
					_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
					_materialInstance->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				std::array<float, 5 * 2> vertices = {
					boxCollider2d->GetOffset().GetX() - boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetOffset().GetY() + boxCollider2d->GetSize().GetY() * 0.5f,
					boxCollider2d->GetOffset().GetX() + boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetOffset().GetY() + boxCollider2d->GetSize().GetY() * 0.5f,
					boxCollider2d->GetOffset().GetX() + boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetOffset().GetY() - boxCollider2d->GetSize().GetY() * 0.5f,
					boxCollider2d->GetOffset().GetX() - boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetOffset().GetY() - boxCollider2d->GetSize().GetY() * 0.5f,
					boxCollider2d->GetOffset().GetX() - boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetOffset().GetY() + boxCollider2d->GetSize().GetY() * 0.5f,
				};

				renderer::RenderCommandMesh* renderMeshCommand = new renderer::RenderCommandMesh(vertices.data(), (uint32_t)vertices.size() / 2, sizeof(float) * 2, nullptr, 0, node2D->GetWorldMatrix(), _materialInstance);
				renderQueue.PushRenderCommand(renderMeshCommand);		
			}
		}
		return false;
	}
}
