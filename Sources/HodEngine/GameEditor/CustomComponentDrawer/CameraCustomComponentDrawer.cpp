#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CameraCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Game/Components/CameraComponent.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Entity.hpp>

#undef max

namespace hod::inline editor
{
	/// @brief
	CameraCustomComponentDrawer::CameraCustomComponentDrawer()
	{
		_materialInstance = Renderer::GetInstance()->CreateMaterialInstance(
			MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("ubo.color", Vector4(0.8f, 0.8f, 0.8f, 1.0f));
	}

	/// @brief
	CameraCustomComponentDrawer::~CameraCustomComponentDrawer()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool CameraCustomComponentDrawer::OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		CameraComponent* cameraComponent = static_cast<CameraComponent*>(component);
		if (cameraComponent != nullptr)
		{
			Node2dComponent* node2D = cameraComponent->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				cameraComponent->SetAspect((float)viewport.GetPlayRatio().GetX() / (float)viewport.GetPlayRatio().GetY());
				const hod::Matrix4& projectionMatrix = cameraComponent->GetProjectionMatrix();

				float width = 2.0f / projectionMatrix[0][0];
				float height = 2.0f / projectionMatrix[1][1];

				std::array<Vector2, 5> vertices = {
					Vector2(-width * 0.5f, height * 0.5f),  Vector2(width * 0.5f, height * 0.5f),  Vector2(width * 0.5f, -height * 0.5f),
					Vector2(-width * 0.5f, -height * 0.5f), Vector2(-width * 0.5f, height * 0.5f),
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
