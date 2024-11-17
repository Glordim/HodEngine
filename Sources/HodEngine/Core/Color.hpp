#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include <cstddef>
#include <cstdint>

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_CORE_API Color final
	{
		REFLECTED_CLASS_NO_VIRTUAL(Color)

	public:

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
