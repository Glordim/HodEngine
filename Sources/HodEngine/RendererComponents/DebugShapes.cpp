#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/DebugShapes.hpp"

#include <utility>

namespace hod::inline renderer
{
	/// @brief
	void DebugShapes::Line(const Vector2& start, const Vector2& end, const Color& color, float duration)
	{
		_shapes.Line(start, end, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::Polyline(const Vector2* points, uint32_t pointCount, bool closed, const Color& color, float duration)
	{
		_shapes.Polyline(points, pointCount, closed, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::Rect(const Vector2& center, const Vector2& size, const Color& color, float duration)
	{
		_shapes.Rect(center, size, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::FilledRect(const Vector2& center, const Vector2& size, const Color& color, float duration)
	{
		_shapes.FilledRect(center, size, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::FilledPolygon(const Vector2* points, uint32_t pointCount, const Color& color, float duration)
	{
		_shapes.FilledPolygon(points, pointCount, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::Circle(const Vector2& center, float radius, const Color& color, float duration)
	{
		_shapes.Circle(center, radius, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::FilledCircle(const Vector2& center, float radius, const Color& color, float duration)
	{
		_shapes.FilledCircle(center, radius, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::Capsule(const Vector2& start, const Vector2& end, float radius, const Color& color, float duration)
	{
		_shapes.Capsule(start, end, radius, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::FilledCapsule(const Vector2& start, const Vector2& end, float radius, const Color& color, float duration)
	{
		_shapes.FilledCapsule(start, end, radius, color);
		SetDuration(duration);
	}

	/// @brief
	void DebugShapes::Point(const Vector2& position, float pixelSize, const Color& color, float duration)
	{
		_shapes.Point(position, pixelSize, color);
		SetDuration(duration);
	}

	/// @brief
	const ShapeList& DebugShapes::GetShapes() const
	{
		return _shapes;
	}

	/// @brief
	const Vector<float>& DebugShapes::GetDurations() const
	{
		return _durations;
	}

	/// @brief
	void DebugShapes::Clear()
	{
		_shapes.Clear();
		_durations.Clear();
		_agedCount = 0;
	}

	/// @brief
	/// @param elapsed
	void DebugShapes::Age(float elapsed)
	{
		if (_durations.Empty())
		{
			return;
		}

		ShapeList     keptShapes;
		Vector<float> keptDurations;

		const Vector<ShapeList::Shape>& shapes = _shapes.GetShapes();
		const Vector2*                  points = _shapes.GetPoints().Data();
		for (uint32_t index = 0; index < shapes.Size(); ++index)
		{
			bool  added = (index >= _agedCount); // since the previous Age: not drawn yet, kept as they are
			float duration = added ? _durations[index] : _durations[index] - elapsed;
			if (added || duration > 0.0f)
			{
				keptShapes.Add(shapes[index], points + shapes[index]._firstPoint);
				keptDurations.PushBack(duration);
			}
		}

		_shapes = std::move(keptShapes);
		_durations = std::move(keptDurations);
		_agedCount = (uint32_t)_durations.Size();
	}

	/// @brief
	void DebugShapes::SetDuration(float duration)
	{
		while (_durations.Size() < _shapes.GetShapes().Size())
		{
			_durations.PushBack(duration);
		}
	}
}
