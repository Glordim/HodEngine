#pragma once

#include <cstdint>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct P2fT2f
		{
			P2fT2f(float x, float y, float u, float v)
			{
				_position[0] = x;
				_position[1] = y;
/*
				_textCoord[0] = u;
				_textCoord[1] = v;
				*/
			}

			float	_position[2];
			//float	_textCoord[2];
		};
	}
}
