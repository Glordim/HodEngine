#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline renderer
{
	class RenderView;
	class ShapeList;

	/// @brief Simple 2d shapes of one color (outlines and fills) pushed into a RenderView: the debug draw of the engine, usable everywhere (game, editor).
	/// Drawing only: the interactive handles of the editor are editor::Handles, built on it.
	/// The points are in the space of matrix (Matrix4::Identity: world space), the outlines are 1 pixel wide, the color is in the vertices (one shared material per
	/// topology, no MaterialInstance per call), the visualization mode (wireframe...) is ignored.
	/// order: rendering order in the RenderView, higher is drawn later (on top), the commands of the same order are drawn in the order they were pushed.
	/// pickingId: what the picking pass writes for the fills (PickingManager), PickingManager::InvalidId: not pickable.
	class HOD_RENDERER_API ShapeDrawer
	{
	public:
		static constexpr uint32_t DefaultCircleSegmentCount = 32;

	public:
		// Outlines

		static void Line(RenderView& renderView, const Matrix4& matrix, const Vector2& start, const Vector2& end, const Color& color, uint32_t order);
		/// @brief Segments: points[0]-points[1], points[2]-points[3]...
		static void Lines(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order);
		/// @brief closed: with the segment from the last point back to the first
		static void Polyline(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, bool closed, const Color& color, uint32_t order);
		static void Rect(RenderView& renderView, const Matrix4& matrix, const Vector2& center, const Vector2& size, const Color& color, uint32_t order);
		static void Circle(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float radius, const Color& color, uint32_t order,
		                   uint32_t segmentCount = DefaultCircleSegmentCount);
		/// @brief Vertical, height is the whole capsule (its two half circles included)
		static void Capsule(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float height, float radius, const Color& color, uint32_t order,
		                    uint32_t segmentCount = DefaultCircleSegmentCount);

		// Fills

		static void FilledRect(RenderView& renderView, const Matrix4& matrix, const Vector2& center, const Vector2& size, const Color& color, uint32_t order,
		                       uint32_t pickingId = 0);
		static void FilledCircle(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float radius, const Color& color, uint32_t order, uint32_t pickingId = 0,
		                         uint32_t segmentCount = DefaultCircleSegmentCount);
		/// @brief Vertical, height is the whole capsule (its two half circles included)
		static void FilledCapsule(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float height, float radius, const Color& color, uint32_t order,
		                          uint32_t pickingId = 0, uint32_t segmentCount = DefaultCircleSegmentCount);
		/// @brief Convex polygon (filled as a fan from its first point)
		static void FilledPolygon(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order,
		                          uint32_t pickingId = 0);
		/// @brief Triangle list, indexed when indices isn't null
		static void FilledTriangles(RenderView& renderView, const Matrix4& matrix, const Vector2* vertices, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount,
		                            const Color& color, uint32_t order, uint32_t pickingId = 0);
		/// @brief The band between two axis-aligned rects (outer contains inner), e.g. a padding or a margin. Rects as {bottom-left corner, size}
		static void FilledFrame(RenderView& renderView, const Matrix4& matrix, const Vector2& outerPosition, const Vector2& outerSize, const Vector2& innerPosition,
		                        const Vector2& innerSize, const Color& color, uint32_t order);

		/// @brief A square of pixelSize pixels whatever the zoom, at position in world space (camera of the RenderView set up first)
		static void Point(RenderView& renderView, const Vector2& position, float pixelSize, const Color& color, uint32_t order);

		/// @brief All the shapes of the list (world space), batched: one command for the fills then one for the outlines (over them), while they fit in 16 bits indices.
		/// The camera of the RenderView set up first (for the points)
		static void Draw(RenderView& renderView, const ShapeList& shapes, uint32_t order);

		/// @brief The size of a pixel in world units with the camera of the RenderView (orthographic), 0 without camera
		static float GetPixelWorldSize(const RenderView& renderView);
	};
}
