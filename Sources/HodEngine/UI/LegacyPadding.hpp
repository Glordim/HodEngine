#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Math/Vector4.hpp>

namespace hod::inline ui
{
	class HOD_UI_API LegacyPadding final
	{
		REFLECTED_CLASS_NO_VIRTUAL(LegacyPadding)

	public:
		
		void	SetLeft(float left);
		float	GetLeft() const;

		void	SetBottom(float bottom);
		float	GetBottom() const;

		void	SetRight(float right);
		float	GetRight() const;

		void	SetTop(float top);
		float	GetTop() const;

		Vector4	_vector4;
	};
}
