#pragma once
#include <HodEngine/HodEngine.hpp>

#include <cstddef>

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_API Color
	{
		Color();
		Color(float r, float g, float b, float a);
		virtual ~Color();

		float& operator[](size_t index)
		{
			if (index == 0) return r;
			if (index == 1) return g;
			if (index == 2) return b;
			if (index == 3) return a;
			return r;
		}

		float r;
		float g;
		float b;
		float a;
	};
}
