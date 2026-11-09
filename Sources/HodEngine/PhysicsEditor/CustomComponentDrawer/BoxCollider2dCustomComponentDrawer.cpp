#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/BoxCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Editor/Gizmos/Gizmos.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/BoxCollider2dComponent.hpp>
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
	bool BoxCollider2dCustomComponentDrawer::OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		BoxCollider2dComponent* boxCollider2d = static_cast<BoxCollider2dComponent*>(component);
		if (boxCollider2d != nullptr)
		{
			Node2dComponent* node2D = boxCollider2d->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				Matrix4 localMatrix = Matrix4::Translation(boxCollider2d->GetOffset()) * Matrix4::Rotation(boxCollider2d->GetRotation());
				ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix() * localMatrix, Vector2::Zero, boxCollider2d->GetSize(), Color(0.0f, 1.0f, 0.0f, 1.0f),
				                  Gizmos::OverlayOrder);
			}
		}
		return false;
	}
}
