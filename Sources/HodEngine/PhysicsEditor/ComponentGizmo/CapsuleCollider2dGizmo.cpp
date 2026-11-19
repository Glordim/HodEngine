#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/ComponentGizmo/CapsuleCollider2dGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param capsuleCollider2d
	/// @param viewport
	/// @param active
	/// @return
	bool CapsuleCollider2dGizmo::OnDraw(CapsuleCollider2dComponent& capsuleCollider2d, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = capsuleCollider2d.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			Vector2 scale = node2D->GetScale();

			Matrix4 worldMatrix = node2D->GetWorldMatrix();
			worldMatrix.RemoveScale();

			Matrix4 localMatrix = Matrix4::Translation(capsuleCollider2d.GetOffset() * scale) * Matrix4::Rotation(capsuleCollider2d.GetRotation());

			ShapeDrawer::Capsule(*viewport.GetRenderView(), worldMatrix * localMatrix, Vector2::Zero, capsuleCollider2d.GetHeight() * scale.GetY(),
			                     capsuleCollider2d.GetRadius() * scale.GetX(), Color(0.0f, 1.0f, 0.0f, 1.0f), GizmoViewport::OverlayOrder, 64);
		}
		return false;
	}
}
