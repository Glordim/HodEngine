#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/EdgeCollider2dComponentCustomEditor.hpp"

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

namespace hod::editor
{
	/// @brief
	EdgeCollider2dComponentCustomEditor::EdgeCollider2dComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("ubo.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief
	EdgeCollider2dComponentCustomEditor::~EdgeCollider2dComponentCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool EdgeCollider2dComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		game::EdgeCollider2dComponent* edgeCollider2d = static_cast<game::EdgeCollider2dComponent*>(component);
		if (edgeCollider2d != nullptr)
		{
			game::Node2dComponent* node2D = edgeCollider2d->GetOwner()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				std::array<Vector2, 2> vertices = {
					edgeCollider2d->GetStart(),
					edgeCollider2d->GetEnd(),
				};

				renderer::RenderCommandMesh* renderMeshCommand =
					DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0,
				                                                                     node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
