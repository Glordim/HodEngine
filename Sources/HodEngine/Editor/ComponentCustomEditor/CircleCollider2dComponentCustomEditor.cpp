#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CircleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp>

namespace hod::editor
{
	/// @brief 
	CircleCollider2dComponentCustomEditor::CircleCollider2dComponentCustomEditor()
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
	bool CircleCollider2dComponentCustomEditor::OnDrawGizmo(std::shared_ptr<game::Component> component, ViewportWindow& viewport)
	{
		std::shared_ptr<game::CircleCollider2dComponent> circleCollider2d = std::static_pointer_cast<game::CircleCollider2dComponent>(component);
		if (circleCollider2d != nullptr)
		{
			std::shared_ptr<game::Node2dComponent> node2D = circleCollider2d->GetEntity()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				std::array<Vector2, 65> vertices;
				GeometryGenerator::CircleShape<64>(vertices, circleCollider2d->GetOffset(), circleCollider2d->GetRadius());

				renderer::RenderCommandMesh* renderMeshCommand = new renderer::RenderCommandMesh(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node2D->GetWorldMatrix(), _materialInstance);
				renderer::RenderQueue::GetInstance()->PushRenderCommand(renderMeshCommand);		
			}
		}
		return false;
	}
}