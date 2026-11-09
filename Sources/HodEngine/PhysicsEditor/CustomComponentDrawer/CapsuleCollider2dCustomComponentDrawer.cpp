#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CapsuleCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/CapsuleCollider2dComponent.hpp>
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
	bool CapsuleCollider2dCustomComponentDrawer::OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		CapsuleCollider2dComponent* capsuleCollider2d = static_cast<CapsuleCollider2dComponent*>(component);
		if (capsuleCollider2d != nullptr)
		{
			Node2dComponent* node2D = capsuleCollider2d->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				Vector2 scale = node2D->GetScale();

				Matrix4 worldMatrix = node2D->GetWorldMatrix();
				worldMatrix.RemoveScale();

				Matrix4 localMatrix = Matrix4::Translation(capsuleCollider2d->GetOffset() * scale) * Matrix4::Rotation(capsuleCollider2d->GetRotation());

				ShapeDrawer::Capsule(*viewport.GetRenderView(), worldMatrix * localMatrix, Vector2::Zero, capsuleCollider2d->GetHeight() * scale.GetY(),
				                     capsuleCollider2d->GetRadius() * scale.GetX(), Color(0.0f, 1.0f, 0.0f, 1.0f), GizmoViewport::OverlayOrder, 64);
			}
		}
		return false;
	}
}
