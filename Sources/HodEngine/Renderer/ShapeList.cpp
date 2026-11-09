#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/ShapeList.hpp"

namespace hod::inline renderer
{
	namespace
	{
		void GetRectPoints(Vector2 (&points)[4], const Vector2& center, const Vector2& size)
		{
			Vector2 halfSize = size * 0.5f;
			points[0] = center + Vector2(-halfSize.GetX(), halfSize.GetY());
			points[1] = center + Vector2(halfSize.GetX(), halfSize.GetY());
			points[2] = center + Vector2(halfSize.GetX(), -halfSize.GetY());
			points[3] = center + Vector2(-halfSize.GetX(), -halfSize.GetY());
		}
	}

	/// @brief
	void ShapeList::Line(const Vector2& start, const Vector2& end, const Color& color)
	{
		Vector2 points[2] = {start, end};
		Add(Type::Polyline, points, 2, 0.0f, color);
	}

	/// @brief
	void ShapeList::Polyline(const Vector2* points, uint32_t pointCount, bool closed, const Color& color)
	{
		if (pointCount >= 2)
		{
			Add(closed ? Type::ClosedPolyline : Type::Polyline, points, pointCount, 0.0f, color);
		}
	}

	/// @brief
	void ShapeList::Rect(const Vector2& center, const Vector2& size, const Color& color)
	{
		Vector2 points[4];
		GetRectPoints(points, center, size);
		Add(Type::ClosedPolyline, points, 4, 0.0f, color);
	}

	/// @brief
	void ShapeList::FilledRect(const Vector2& center, const Vector2& size, const Color& color)
	{
		Vector2 points[4];
		GetRectPoints(points, center, size);
		Add(Type::FilledPolygon, points, 4, 0.0f, color);
	}

	/// @brief
	void ShapeList::FilledPolygon(const Vector2* points, uint32_t pointCount, const Color& color)
	{
		if (pointCount >= 3)
		{
			Add(Type::FilledPolygon, points, pointCount, 0.0f, color);
		}
	}

	/// @brief
	void ShapeList::Circle(const Vector2& center, float radius, const Color& color)
	{
		Add(Type::Circle, &center, 1, radius, color);
	}

	/// @brief
	void ShapeList::FilledCircle(const Vector2& center, float radius, const Color& color)
	{
		Add(Type::FilledCircle, &center, 1, radius, color);
	}

	/// @brief
	void ShapeList::Capsule(const Vector2& start, const Vector2& end, float radius, const Color& color)
	{
		Vector2 points[2] = {start, end};
		Add(Type::Capsule, points, 2, radius, color);
	}

	/// @brief
	void ShapeList::FilledCapsule(const Vector2& start, const Vector2& end, float radius, const Color& color)
	{
		Vector2 points[2] = {start, end};
		Add(Type::FilledCapsule, points, 2, radius, color);
	}

	/// @brief
	void ShapeList::Point(const Vector2& position, float pixelSize, const Color& color)
	{
		Add(Type::Point, &position, 1, pixelSize, color);
	}

	/// @brief
	void ShapeList::Add(const Shape& shape, const Vector2* points)
	{
		Add(shape._type, points, shape._pointCount, shape._size, shape._color);
	}

	/// @brief
	const Vector<ShapeList::Shape>& ShapeList::GetShapes() const
	{
		return _shapes;
	}

	/// @brief
	const Vector<Vector2>& ShapeList::GetPoints() const
	{
		return _points;
	}

	/// @brief
	bool ShapeList::Empty() const
	{
		return _shapes.Empty();
	}

	/// @brief
	void ShapeList::Clear()
	{
		_shapes.Clear();
		_points.Clear();
	}

	/// @brief
	void ShapeList::Add(Type type, const Vector2* points, uint32_t pointCount, float size, const Color& color)
	{
		Shape shape;
		shape._type = type;
		shape._color = color;
		shape._size = size;
		shape._firstPoint = (uint32_t)_points.Size();
		shape._pointCount = pointCount;
		_shapes.PushBack(shape);

		for (uint32_t index = 0; index < pointCount; ++index)
		{
			_points.PushBack(points[index]);
		}
	}
}
