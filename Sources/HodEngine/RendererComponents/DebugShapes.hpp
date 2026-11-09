#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include <HodEngine/Renderer/ShapeList.hpp>

namespace hod::inline renderer
{
	/// @brief The shapes drawn to debug a game, each for a duration: those of a World, RendererWorldSystem::GetDebugShapes(world), drawn over its content,
	/// aged with the world's updates (game time: kept while paused).
	/// duration: in seconds, 0 for a single frame (until the next update of the world)
	// todo #ifndef retail ?
	class HOD_RENDERER_COMPONENTS_API DebugShapes
	{
	public:
		void Line(const Vector2& start, const Vector2& end, const Color& color, float duration = 0.0f);
		void Polyline(const Vector2* points, uint32_t pointCount, bool closed, const Color& color, float duration = 0.0f);
		void Rect(const Vector2& center, const Vector2& size, const Color& color, float duration = 0.0f);
		void FilledRect(const Vector2& center, const Vector2& size, const Color& color, float duration = 0.0f);
		/// @brief Convex polygon
		void FilledPolygon(const Vector2* points, uint32_t pointCount, const Color& color, float duration = 0.0f);
		void Circle(const Vector2& center, float radius, const Color& color, float duration = 0.0f);
		void FilledCircle(const Vector2& center, float radius, const Color& color, float duration = 0.0f);
		/// @brief start, end: the centers of its two half circles
		void Capsule(const Vector2& start, const Vector2& end, float radius, const Color& color, float duration = 0.0f);
		void FilledCapsule(const Vector2& start, const Vector2& end, float radius, const Color& color, float duration = 0.0f);
		/// @brief A square of pixelSize pixels whatever the zoom
		void Point(const Vector2& position, float pixelSize, const Color& color, float duration = 0.0f);

		const ShapeList& GetShapes() const;
		/// @brief Left, in seconds, one per shape of GetShapes()
		const Vector<float>& GetDurations() const;

		void Clear();
		/// @brief Once per frame, between two draws: removes the shapes whose duration is over once elapsed is spent, the others keep their order.
		/// The shapes added since the previous Age aren't aged yet (not drawn yet), whether they were added before or after this call in the frame
		void Age(float elapsed);

	private:
		/// @brief The duration of what the last call added to _shapes (nothing for a degenerate shape)
		void SetDuration(float duration);

	private:
		ShapeList     _shapes;
		Vector<float> _durations;
		uint32_t      _agedCount = 0; // The first shapes, there at the previous Age
	};
}
