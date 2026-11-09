#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Color.hpp>

#include <limits>

#undef max

namespace hod::inline editor
{
	class GizmoViewport;

	/// @brief The state of a handle, kept from frame to frame by its owner (Handles::GenerateHandle once, then a Handles::FreeMove* each frame)
	struct HOD_EDITOR_API Handle
	{
		uint32_t	_pickingId = 0;
		bool 		_pressed = false;
		bool		_justPressed = false;
		bool		_hovered = false;
		bool		_canceled = false;
		Vector2		_initialPosition;
		Vector2		_moveOffset;
		Vector2		_delta;
		uint32_t	_sortingOrder = std::numeric_limits<uint32_t>::max(); // Over the overlays (GizmoViewport::OverlayOrder)
	};

	/// @brief The interactive handles of the editor viewports (picking, hover, drag, cancel with Escape), drawn with renderer::ShapeDrawer.
	/// What only shows something (outlines, frames, grids) uses ShapeDrawer directly, with GizmoViewport::OverlayOrder to stay over the content.
	/// With them, the OnDrawGizmo of the component drawers make the gizmos: what an object shows in the editor viewports (drawing + handles).
	class HOD_EDITOR_API Handles
	{
	public:

		static Handle 	GenerateHandle();
		static float	GetHandleSize(float size, GizmoViewport& viewport);

		static bool FreeMoveCircle(Handle& handle, const Matrix4& worldMatrix, const Vector2& position, float radius, const Color& color, const Color& highlightColor, GizmoViewport& viewport);
		static bool FreeMoveRect(Handle& handle, const Matrix4& worldMatrix, const Vector2& position, const Vector2& size, const Color& color, const Color& highlightColor, GizmoViewport& viewport);
		/// @brief vertices: a triangle list
		static bool FreeMoveMesh(Handle& handle, const Matrix4& worldMatrix, const Vector2* vertices, uint32_t vertexCount, const Color& color, const Color& highlightColor, GizmoViewport& viewport);

	private:

		static bool FreeMoveBehavior(Handle& handle, GizmoViewport& viewport);
		static Vector2 GetMouseWorldPos(const Vector2& mousePosition, const GizmoViewport& viewport);
	};
}
