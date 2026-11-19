#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/ComponentGizmo/CircleCollider2dGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Core/Math.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param circleCollider2d
	/// @param viewport
	/// @param active
	/// @return
	bool CircleCollider2dGizmo::OnDraw(CircleCollider2dComponent& circleCollider2d, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = circleCollider2d.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			Vector2 scale = node2D->GetScale();

			ShapeDrawer::Circle(*viewport.GetRenderView(), node2D->GetWorldMatrix(), circleCollider2d.GetOffset() * scale,
			                    circleCollider2d.GetRadius() * Max(scale.GetX(), scale.GetY()), Color(0.0f, 1.0f, 0.0f, 1.0f), GizmoViewport::OverlayOrder, 64);
		}
		return false;
	}
}
