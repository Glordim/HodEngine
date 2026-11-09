#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DebugLines.hpp"

#include <utility>

namespace hod::inline game
{
	/// @brief
	/// @param start
	/// @param end
	/// @param color
	/// @param duration
	void DebugLines::AddLine(const Vector2& start, const Vector2& end, const Color& color, float duration)
	{
		_lines.EmplaceBack(start, end, color, duration);
	}

	/// @brief
	/// @return
	const Vector<DebugLines::Line>& DebugLines::GetLines() const
	{
		return _lines;
	}

	/// @brief
	/// @param elapsed
	void DebugLines::Age(float elapsed)
	{
		auto it = _lines.Begin();
		auto itEnd = _lines.End();
		while (it != itEnd)
		{
			it->_duration -= elapsed;
			if (it->_duration <= 0.0f)
			{
				std::swap(*it, _lines.Back());
				_lines.PopBack();
				itEnd = _lines.End();
			}
			else
			{
				++it;
			}
		}
	}
}
