#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>

namespace hod::inline math
{
	class Matrix4;
	class Vector2;
}

namespace hod::inline renderer
{
	class RenderView;
	class ReadbackRenderTarget;
}

namespace hod::inline editor
{
	/// @brief What the gizmos (the OnDrawGizmo of the component drawers, Handles) need of a viewport to draw and move their handles with the mouse (picking):
	/// implemented by the viewports that show gizmos
	class HOD_EDITOR_API GizmoViewport
	{
	public:
		/// @brief Rendering order of the editor overlays (outlines, frames...): over the content, under the handles (Handle::_sortingOrder)
		static constexpr uint32_t OverlayOrder = 0xFFFFFFFF - 1;

		virtual ~GizmoViewport() = default;

		virtual RenderView*           GetRenderView() = 0;
		virtual ReadbackRenderTarget* GetPickingRenderTarget() const = 0;
		virtual const Matrix4&        GetProjectionMatrix() const = 0;
		virtual const Matrix4&        GetViewMatrix() const = 0;
		virtual float                 GetCameraSize() const = 0;
		/// @brief Of the game screen previewed in the viewport (eg. for the aspect of a camera)
		virtual const Vector2&        GetPlayRatio() const = 0;

		/// @brief true while the mouse is used by something drawn over the viewport, the gizmos must then ignore it
		virtual bool IsMouseOverToolbar() const = 0;
	};
}
