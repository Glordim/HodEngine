#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/ComponentGizmo/CameraGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param camera
	/// @param viewport
	/// @param active
	/// @return
	bool CameraGizmo::OnDraw(CameraComponent& camera, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = camera.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			camera.SetAspect((float)viewport.GetPlayRatio().GetX() / (float)viewport.GetPlayRatio().GetY());
			const Matrix4& projectionMatrix = camera.GetProjectionMatrix();

			float width = 2.0f / projectionMatrix[0][0];
			float height = 2.0f / projectionMatrix[1][1];

			ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix(), Vector2::Zero, Vector2(width, height), Color(0.8f, 0.8f, 0.8f, 1.0f), GizmoViewport::OverlayOrder);
		}
		return false;
	}
}
