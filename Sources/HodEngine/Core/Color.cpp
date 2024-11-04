#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Color.hpp"

namespace hod
{
	Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
	Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);

	DESCRIBE_REFLECTED_CLASS(Color, void)
	{
		ADD_PROPERTY(Color, r);
		ADD_PROPERTY(Color, g);
		ADD_PROPERTY(Color, b);
		ADD_PROPERTY(Color, a);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Color::Color()
		: r(1)
		, g(1)
		, b(1)
		, a(1)
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Color::Color(float r, float g, float b, float a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{
	}

	/// @brief 
	/// @param rgba 
	Color::Color(int32_t rgba)
		: r(((rgba >> 24) & 0xFF) / 255.0f)
		, g(((rgba >> 16) & 0xFF) / 255.0f)
		, b(((rgba >> 8) & 0xFF) / 255.0f)
		, a(((rgba >> 0) & 0xFF) / 255.0f)
	{

	}
}
