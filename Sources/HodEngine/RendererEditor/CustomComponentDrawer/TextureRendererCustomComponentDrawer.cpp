#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/RendererEditor/CustomComponentDrawer/TextureRendererCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Editor/Gizmos/Gizmos.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/RendererComponents/Components/TextureRendererComponent.hpp>
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
	bool TextureRendererCustomComponentDrawer::OnDrawGizmo(Component* component, GizmoViewport& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		TextureRendererComponent* textureRenderer = static_cast<TextureRendererComponent*>(component);
		if (textureRenderer != nullptr)
		{
			Node2dComponent* node2D = textureRenderer->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				Rect bb = textureRenderer->GetBoundingBox();

				ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix(), Vector2::Zero, bb._size, Color(0.75f, 0.75f, 0.75f, 1.0f), Gizmos::OverlayOrder);
			}
		}
		return false;
	}
}
