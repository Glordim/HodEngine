#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Editor/Gizmos/Gizmos.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Core/Math.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/CircleCollider2dComponent.hpp>
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
	bool CircleCollider2dCustomComponentDrawer::OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		CircleCollider2dComponent* circleCollider2d = static_cast<CircleCollider2dComponent*>(component);
		if (circleCollider2d != nullptr)
		{
			Node2dComponent* node2D = circleCollider2d->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				Vector2 scale = node2D->GetScale();

				ShapeDrawer::Circle(*viewport.GetRenderView(), node2D->GetWorldMatrix(), circleCollider2d->GetOffset() * scale,
				                    circleCollider2d->GetRadius() * Max(scale.GetX(), scale.GetY()), Color(0.0f, 1.0f, 0.0f, 1.0f), Gizmos::OverlayOrder, 64);
			}
		}
		return false;
	}
}
