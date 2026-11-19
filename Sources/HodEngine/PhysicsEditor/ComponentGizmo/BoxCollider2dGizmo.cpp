#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/ComponentGizmo/BoxCollider2dGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param boxCollider2d
	/// @param viewport
	/// @param active
	/// @return
	bool BoxCollider2dGizmo::OnDraw(BoxCollider2dComponent& boxCollider2d, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = boxCollider2d.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			Matrix4 localMatrix = Matrix4::Translation(boxCollider2d.GetOffset()) * Matrix4::Rotation(boxCollider2d.GetRotation());
			ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix() * localMatrix, Vector2::Zero, boxCollider2d.GetSize(), Color(0.0f, 1.0f, 0.0f, 1.0f),
			                  GizmoViewport::OverlayOrder);
		}
		return false;
	}
}
