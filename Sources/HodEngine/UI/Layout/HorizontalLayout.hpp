#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Layout/AxisLayout.hpp"

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API HorizontalLayout : public AxisLayout
	{
		REFLECTED_CLASS(HorizontalLayout, AxisLayout)

	public:

		void	ComputeChildrenPositionAndSize() override;
	};
}
