#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/EdgeCollider2dComponent.hpp>
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
	bool EdgeCollider2dCustomComponentDrawer::OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		EdgeCollider2dComponent* edgeCollider2d = static_cast<EdgeCollider2dComponent*>(component);
		if (edgeCollider2d != nullptr)
		{
			Node2dComponent* node2D = edgeCollider2d->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				ShapeDrawer::Line(*viewport.GetRenderView(), node2D->GetWorldMatrix(), edgeCollider2d->GetStart(), edgeCollider2d->GetEnd(), Color(0.0f, 1.0f, 0.0f, 1.0f),
				                  GizmoViewport::OverlayOrder);
			}
		}
		return false;
	}
}
