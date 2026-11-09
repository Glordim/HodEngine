#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline editor
{
	/// @brief The grid of the 2d editor viewports (scenes and prefabs, UI prefabs): drawn with ShapeDrawer, covering the whole visible
	/// area, with a step that adapts to the zoom so that the lines stay readable on screen.
	///
	/// At zoom 1 the major step is exactly cellSize; zooming out doubles it each time the zoom halves (zooming in halves it), and it is
	/// kept above a minimum on-screen spacing so that a tiny cell can't flood the view. The finer subdivision (half the major step) is
	/// faded in as the zoom approaches the point where it becomes the new major step, instead of the grid popping to a denser level.
	struct HOD_EDITOR_API ViewportGrid
	{
		// The rendering order of the grid: under everything else
		static constexpr uint32_t Order = 0;

		// Where the lines are (in world space, the RenderView's camera's)
		Vector2 _origin = Vector2::Zero; // a line always passes through it, on each axis
		float   _cellSize = 1.0f;        // the major step at zoom 1

		// What is visible: the world point at the center of the view, half the view's size in world units, the pixels per world unit
		Vector2 _viewCenter = Vector2::Zero;
		Vector2 _viewHalfSize = Vector2::One;
		float   _pixelsPerUnit = 1.0f;
		// Relative to the viewport's default framing (1: the major step is cellSize)
		float _zoom = 1.0f;

		Color _color = Color(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
		// What the lines are drawn over: the shapes are not blended, the minor lines fade in from this color
		Color _backgroundColor = Color(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f);

		void Draw(RenderView& renderView) const;
	};
}
