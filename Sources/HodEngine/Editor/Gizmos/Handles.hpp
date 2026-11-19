#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>
#include <limits>

namespace hod::inline editor
{
	class GizmoViewport;

	/// @brief Identifies a handle from frame to frame, Handles keeping its state (pressed, hovered, the value when pressed): its owner (the
	/// component it moves a value of...) and its index among the owner's handles
	struct HOD_EDITOR_API HandleId
	{
		HandleId(const void* owner, uint32_t index = 0)
		: _owner(owner)
		, _index(index)
		{
		}

		const void* _owner = nullptr;
		uint32_t    _index = 0;
	};

	/// @brief How a handle looks: its colors (normal, hovered, pressed) and its sorting order. Made from a color (implicit): the hovered and
	/// pressed ones that color lightened (Handles::GetHighlightColor)
	struct HOD_EDITOR_API HandleStyle
	{
		static constexpr uint32_t DefaultSortingOrder = std::numeric_limits<uint32_t>::max(); // Over the overlays (GizmoViewport::OverlayOrder)

		HandleStyle(const Color& color);
		HandleStyle(const Color& color, const Color& hoveredColor, const Color& pressedColor);

		/// @return The color for this state: pressed, hovered or normal
		const Color& GetColor(bool hovered, bool pressed) const;

		Color    _color;
		Color    _hoveredColor;
		Color    _pressedColor;
		uint32_t _sortingOrder = DefaultSortingOrder;
	};

	/// @brief The state of a handle this frame, after its behavior ran (given by the Handles functions on demand)
	struct HOD_EDITOR_API HandleResult
	{
		bool    _hovered = false;
		bool    _pressed = false;      // Being dragged (from its press to its release or Escape)
		bool    _justPressed = false;  // The drag starts this frame
		bool    _justReleased = false; // The drag ends this frame (the mouse released): the moment to commit, regenerate...
		bool    _canceled = false;     // The drag is canceled this frame (Escape): FreeMove* gave its value back, no release follows
		Vector2 _worldDelta;           // While pressed, the mouse's move since the press, in the world (projected on the handle's axis if it has one)
	};

	/// @brief The interactive handles of the editor viewports (picking, hover, drag, cancel with Escape), drawn with renderer::ShapeDrawer.
	/// What only shows something (outlines, frames, grids) uses ShapeDrawer directly, with GizmoViewport::OverlayOrder to stay over the content.
	/// With them, the component gizmos (ComponentGizmo) make what an object shows in the editor viewports (drawing + handles).
	///
	/// A FreeMove* handle moves a value with the mouse: position, in valueSpace (the space its owner stores it in: a component's local position
	/// with its node's world matrix, a node's position with its parent's), follows the drag converted to that space; Escape gives it back the
	/// value it had when pressed. Drag is the same handle without a value (a radius, an angle...: the caller applies its _worldDelta). Their
	/// state is kept by Handles, per viewport and id, while they are drawn every frame; result (optional) gives it.
	class HOD_EDITOR_API Handles
	{
	public:
		/// @brief A circle drawn at position
		/// @return true if position changed
		static bool FreeMoveCircle(GizmoViewport& viewport, const HandleId& id, const Matrix4& valueSpace, Vector2& position, float radius, const HandleStyle& style,
		                           HandleResult* result = nullptr);

		/// @brief A rect of size centered on position
		/// @return true if position changed
		static bool FreeMoveRect(GizmoViewport& viewport, const HandleId& id, const Matrix4& valueSpace, Vector2& position, const Vector2& size, const HandleStyle& style,
		                         HandleResult* result = nullptr);

		/// @brief The general one: a triangle list drawn with drawMatrix (where the handle is shown, not necessarily at position: an arrow beside
		/// it, at a constant size on screen...)
		/// @param worldAxis Not zero: the drag projected on it (a move along an axis, in the world)
		/// @return true if position changed
		static bool FreeMoveMesh(GizmoViewport& viewport, const HandleId& id, const Matrix4& valueSpace, Vector2& position, const Matrix4& drawMatrix, const Vector2* vertices,
		                         uint32_t vertexCount, const HandleStyle& style, const Vector2& worldAxis = Vector2::Zero, HandleResult* result = nullptr);

		/// @brief A handle without a value: a triangle list drawn with drawMatrix, its state given back. The caller keeps its value on
		/// _justPressed and applies _worldDelta to it (a radius, an angle...), gives it back on _canceled
		/// @param worldAxis Not zero: the drag projected on it
		static HandleResult Drag(GizmoViewport& viewport, const HandleId& id, const Matrix4& drawMatrix, const Vector2* vertices, uint32_t vertexCount, const HandleStyle& style,
		                         const Vector2& worldAxis = Vector2::Zero);

		/// @return A world size giving size * 100 pixels at the viewport's zoom: a handle of a constant size on screen
		static float	GetHandleSize(float size, GizmoViewport& viewport);
		/// @return color halfway to white, its alpha kept: a hovered handle's
		static Color	GetHighlightColor(const Color& color);
		/// @return A delta of the world (a drag) in valueSpace: its rotation and scale undone, not its translation
		static Vector2	WorldToLocalDelta(const Matrix4& valueSpace, const Vector2& worldDelta);
		/// @return worldDelta's part along worldAxis (worldDelta itself for a zero axis)
		static Vector2	ProjectOnAxis(const Vector2& worldDelta, const Vector2& worldAxis);

		/// @brief The state of a handle, defined in Handles.cpp only
		struct State;

	private:
		/// @return The state of id in viewport, its behavior run (hover, press, drag, release, Escape), result filled
		static State& Interact(GizmoViewport& viewport, const HandleId& id, const Vector2& worldAxis, HandleResult& result);
		/// @brief position moved by the drag (result) from its value when pressed, given back on Escape
		/// @return true if position changed
		static bool MoveValue(State& state, const HandleResult& result, const Matrix4& valueSpace, Vector2& position);
	};
}
