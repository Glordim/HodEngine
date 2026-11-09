#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace hod::inline renderer
{
	/// @brief Simple 2d shapes of one color in world space, filled then drawn at once (ShapeDrawer::Draw, batched): the many shapes of a debug view
	/// (the physics', the world's DebugShapes). The shapes of ShapeDrawer, kept until Clear
	class HOD_RENDERER_API ShapeList
	{
	public:
		enum class Type : uint8_t
		{
			Polyline,       // open: points[0]-points[1]-...
			ClosedPolyline, // with the segment from the last point back to the first
			FilledPolygon,  // convex
			Circle,         // points[0]: center
			FilledCircle,
			Capsule, // points[0], points[1]: the centers of its two half circles
			FilledCapsule,
			Point, // points[0], _size in pixels
		};

		struct Shape
		{
			Type     _type = Type::Polyline;
			Color    _color;
			float    _size = 0.0f;    // Circle, Capsule: radius. Point: in pixels
			uint32_t _firstPoint = 0; // In GetPoints()
			uint32_t _pointCount = 0;
		};

	public:
		void Line(const Vector2& start, const Vector2& end, const Color& color);
		/// @brief Ignored under 2 points
		void Polyline(const Vector2* points, uint32_t pointCount, bool closed, const Color& color);
		void Rect(const Vector2& center, const Vector2& size, const Color& color);
		void FilledRect(const Vector2& center, const Vector2& size, const Color& color);
		/// @brief Convex polygon, ignored under 3 points
		void FilledPolygon(const Vector2* points, uint32_t pointCount, const Color& color);
		void Circle(const Vector2& center, float radius, const Color& color);
		void FilledCircle(const Vector2& center, float radius, const Color& color);
		/// @brief start, end: the centers of its two half circles
		void Capsule(const Vector2& start, const Vector2& end, float radius, const Color& color);
		void FilledCapsule(const Vector2& start, const Vector2& end, float radius, const Color& color);
		/// @brief A square of pixelSize pixels whatever the zoom
		void Point(const Vector2& position, float pixelSize, const Color& color);

		/// @brief A shape as given by GetShapes (its _firstPoint ignored), points: its _pointCount points
		void Add(const Shape& shape, const Vector2* points);

		const Vector<Shape>&   GetShapes() const;
		const Vector<Vector2>& GetPoints() const;
		bool                   Empty() const;

		void Clear();

	private:
		void Add(Type type, const Vector2* points, uint32_t pointCount, float size, const Color& color);

	private:
		Vector<Shape>   _shapes;
		Vector<Vector2> _points;
	};
}
