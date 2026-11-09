#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/RendererEditor/CustomComponentDrawer/CameraCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Editor/Gizmos/Gizmos.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <HodEngine/RendererComponents/Components/CameraComponent.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Core/StaticArray.hpp>

#undef max

namespace hod::inline editor
{
	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool CameraCustomComponentDrawer::OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected)
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

				ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix(), Vector2::Zero, Vector2(width, height), Color(0.8f, 0.8f, 0.8f, 1.0f), Gizmos::OverlayOrder);
			}
		}
		return false;
	}
}
