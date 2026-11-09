#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/ViewportGrid.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <cmath>

namespace hod::inline editor
{
	namespace
	{
		/// @brief The lines of one axis every step from origin, over [min, max] (along the axis) and [crossMin, crossMax] (across),
		/// every other one only when skipEven (the ones a coarser level draws)
		void CollectLines(Vector<Vector2>& points, bool vertical, float origin, float step, float min, float max, float crossMin, float crossMax, bool skipEven)
		{
			int64_t first = (int64_t)std::floor((min - origin) / step);
			int64_t last = (int64_t)std::ceil((max - origin) / step);
			for (int64_t i = first; i <= last; ++i)
			{
				if (skipEven && i % 2 == 0)
				{
					continue;
				}
				float position = origin + (float)i * step;
				if (vertical)
				{
					points.PushBack(Vector2(position, crossMin));
					points.PushBack(Vector2(position, crossMax));
				}
				else
				{
					points.PushBack(Vector2(crossMin, position));
					points.PushBack(Vector2(crossMax, position));
				}
			}
		}
	}

	/// @brief
	/// @param renderView
	void ViewportGrid::Draw(RenderView& renderView) const
	{
		constexpr float minScreenSpacing = 16.0f;

		if (_pixelsPerUnit <= 0.0f)
		{
			return;
		}

		float cellSize = Max(_cellSize, 1e-3f);

		// Level = number of doublings of the cell size: 0 at zoom 1, +1 each time the zoom halves
		float zoomLevel = -std::log2(Max(_zoom, 1e-6f));
		float minLevel = std::log2(minScreenSpacing / (cellSize * _pixelsPerUnit));
		float level = Max(zoomLevel, minLevel);
		float majorLevel = std::ceil(level);
		float majorStep = cellSize * std::pow(2.0f, majorLevel);
		float minorStep = majorStep * 0.5f;

		// 0 right after a level switch, ramping up to 1 as the minor grid is about to become the new major one
		float minorFade = majorLevel - level;
		Color minorColor(_backgroundColor.r + (_color.r - _backgroundColor.r) * minorFade, _backgroundColor.g + (_color.g - _backgroundColor.g) * minorFade,
		                 _backgroundColor.b + (_color.b - _backgroundColor.b) * minorFade, 1.0f);

		float left = _viewCenter.GetX() - _viewHalfSize.GetX();
		float right = _viewCenter.GetX() + _viewHalfSize.GetX();
		float bottom = _viewCenter.GetY() - _viewHalfSize.GetY();
		float top = _viewCenter.GetY() + _viewHalfSize.GetY();

		// Minor lines first (the half steps, not those of a major line), the major ones over them
		Vector<Vector2> points;
		CollectLines(points, true, _origin.GetX(), minorStep, left, right, bottom, top, true);
		CollectLines(points, false, _origin.GetY(), minorStep, bottom, top, left, right, true);
		if (points.Empty() == false)
		{
			ShapeDrawer::Lines(renderView, Matrix4::Identity, points.Data(), (uint32_t)points.Size(), minorColor, Order);
		}

		points.Clear();
		CollectLines(points, true, _origin.GetX(), majorStep, left, right, bottom, top, false);
		CollectLines(points, false, _origin.GetY(), majorStep, bottom, top, left, right, false);
		if (points.Empty() == false)
		{
			ShapeDrawer::Lines(renderView, Matrix4::Identity, points.Data(), (uint32_t)points.Size(), _color, Order);
		}
	}
}
