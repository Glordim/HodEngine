#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/ComponentGizmo/TextureRendererGizmo.hpp"

#include <HodEngine/Editor/Gizmos/GizmoViewport.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param textureRenderer
	/// @param viewport
	/// @param active
	/// @return
	bool TextureRendererGizmo::OnDraw(TextureRendererComponent& textureRenderer, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		Node2dComponent* node2D = textureRenderer.GetComponent<Node2dComponent>();
		if (node2D != nullptr)
		{
			Rect bb = textureRenderer.GetBoundingBox();

			ShapeDrawer::Rect(*viewport.GetRenderView(), node2D->GetWorldMatrix(), Vector2::Zero, bb._size, Color(0.75f, 0.75f, 0.75f, 1.0f), GizmoViewport::OverlayOrder);
		}
		return false;
	}
}
