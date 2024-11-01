#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstddef>

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_CORE_API Color final
	{
		static Color White;
		static Color Black;

		Color();
		Color(float r, float g, float b, float a);
		Color(int32_t rgba);

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
