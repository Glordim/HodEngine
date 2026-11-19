#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/ComponentGizmo/MultiShapeCollider2dGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Core/Math.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param multiShapeCollider2d
	/// @param viewport
	/// @param active
	/// @return
	bool MultiShapeCollider2dGizmo::OnDraw(MultiShapeCollider2dComponent& multiShapeCollider2d, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = multiShapeCollider2d.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			Vector2     scale = node2D->GetScale();
			const Color color(0.0f, 1.0f, 0.0f, 1.0f);

			for (const MultiShapeCollider2dComponent::BoxShape& boxShape : multiShapeCollider2d.GetBoxShapes())
			{
				Matrix4 localMatrix = Matrix4::Translation(boxShape._origin) * Matrix4::Rotation(boxShape._angle);
				ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix() * localMatrix, Vector2::Zero, boxShape._size, color, GizmoViewport::OverlayOrder);
			}

			for (const MultiShapeCollider2dComponent::CircleShape& circleShape : multiShapeCollider2d.GetCircleShapes())
			{
				ShapeDrawer::Circle(*viewport.GetRenderView(), node2D->GetWorldMatrix(), circleShape._origin * scale, circleShape._radius * Max(scale.GetX(), scale.GetY()),
				                    color, GizmoViewport::OverlayOrder, 64);
			}
		}
		return false;
	}
}
