#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/ComponentGizmo/EdgeCollider2dGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param edgeCollider2d
	/// @param viewport
	/// @param active
	/// @return
	bool EdgeCollider2dGizmo::OnDraw(EdgeCollider2dComponent& edgeCollider2d, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = edgeCollider2d.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			ShapeDrawer::Line(*viewport.GetRenderView(), node2D->GetWorldMatrix(), edgeCollider2d.GetStart(), edgeCollider2d.GetEnd(), Color(0.0f, 1.0f, 0.0f, 1.0f),
			                  GizmoViewport::OverlayOrder);
		}
		return false;
	}
}
