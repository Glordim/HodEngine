#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CircleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp>

#undef max

namespace hod::editor
{
	/// @brief 
	CircleCollider2dComponentCustomEditor::CircleCollider2dComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief 
	CircleCollider2dComponentCustomEditor::~CircleCollider2dComponentCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool CircleCollider2dComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport)
	{
		game::CircleCollider2dComponent* circleCollider2d = static_cast<game::CircleCollider2dComponent*>(component);
		if (circleCollider2d != nullptr)
		{
			game::Node2dComponent* node2D = circleCollider2d->GetOwner()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				Vector2 scale = node2D->GetScale();

				std::array<Vector2, 65> vertices;
				GeometryGenerator::CircleShape<64>(vertices, circleCollider2d->GetOffset() * scale, circleCollider2d->GetRadius() * std::max(scale.GetX(), scale.GetY()));

				renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
