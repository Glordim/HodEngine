#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/EdgeCollider2dComponent.hpp>
#include <HodEngine/Game/Entity.hpp>

#undef max

namespace hod::inline editor
{
	/// @brief
	EdgeCollider2dCustomComponentDrawer::EdgeCollider2dCustomComponentDrawer()
	{
		_materialInstance = Renderer::GetInstance()->CreateMaterialInstance(
			MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("ubo.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief
	EdgeCollider2dCustomComponentDrawer::~EdgeCollider2dCustomComponentDrawer()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool EdgeCollider2dCustomComponentDrawer::OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		EdgeCollider2dComponent* edgeCollider2d = static_cast<EdgeCollider2dComponent*>(component);
		if (edgeCollider2d != nullptr)
		{
			Node2dComponent* node2D = edgeCollider2d->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				std::array<Vector2, 2> vertices = {
					edgeCollider2d->GetStart(),
					edgeCollider2d->GetEnd(),
				};

				RenderCommandMesh* renderMeshCommand =
					DefaultAllocator::GetInstance().New<RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0,
				                                                                     node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
