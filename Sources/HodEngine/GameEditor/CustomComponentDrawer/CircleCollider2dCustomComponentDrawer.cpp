#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp>
#include <HodEngine/Game/Entity.hpp>

#undef max

namespace hod::inline editor
{
	/// @brief
	CircleCollider2dCustomComponentDrawer::CircleCollider2dCustomComponentDrawer()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("ubo.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief
	CircleCollider2dCustomComponentDrawer::~CircleCollider2dCustomComponentDrawer()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool CircleCollider2dCustomComponentDrawer::OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		CircleCollider2dComponent* circleCollider2d = static_cast<CircleCollider2dComponent*>(component);
		if (circleCollider2d != nullptr)
		{
			Node2dComponent* node2D = circleCollider2d->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				Vector2 scale = node2D->GetScale();

				std::array<Vector2, 65> vertices;
				GeometryGenerator::CircleShape<64>(vertices, circleCollider2d->GetOffset() * scale, circleCollider2d->GetRadius() * std::max(scale.GetX(), scale.GetY()));

				renderer::RenderCommandMesh* renderMeshCommand =
					DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0,
				                                                                     node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
