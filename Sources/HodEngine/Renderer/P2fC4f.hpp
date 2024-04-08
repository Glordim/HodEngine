#pragma once

#include <cstdint>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		struct P2fC4f
		{
			P2fC4f() = default;

			P2fC4f(float x, float y, float r, float g, float b, float a)
				: _position{x, y}
				, _color{r, g, b, a}
			{
			}

			float	_position[2] = { 0.0f, 0.0f };
			float	_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		};
	}
}
