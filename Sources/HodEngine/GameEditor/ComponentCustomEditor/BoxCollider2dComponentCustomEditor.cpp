#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/BoxCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp>

#undef max

namespace hod::editor
{
	/// @brief 
	BoxCollider2dComponentCustomEditor::BoxCollider2dComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief 
	BoxCollider2dComponentCustomEditor::~BoxCollider2dComponentCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool BoxCollider2dComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		game::BoxCollider2dComponent* boxCollider2d = static_cast<game::BoxCollider2dComponent*>(component);
		if (boxCollider2d != nullptr)
		{
			game::Node2dComponent* node2D = boxCollider2d->GetOwner()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				std::array<Vector2, 5> vertices = {
					Vector2(-boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetSize().GetY() * 0.5f),
					Vector2(boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetSize().GetY() * 0.5f),
					Vector2(boxCollider2d->GetSize().GetX() * 0.5f, -boxCollider2d->GetSize().GetY() * 0.5f),
					Vector2(-boxCollider2d->GetSize().GetX() * 0.5f, -boxCollider2d->GetSize().GetY() * 0.5f),
					Vector2(-boxCollider2d->GetSize().GetX() * 0.5f, boxCollider2d->GetSize().GetY() * 0.5f),
				};

				Matrix4 localMatrix = Matrix4::Translation(boxCollider2d->GetOffset()) * Matrix4::Rotation(boxCollider2d->GetRotation());

				renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node2D->GetWorldMatrix() * localMatrix, _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
