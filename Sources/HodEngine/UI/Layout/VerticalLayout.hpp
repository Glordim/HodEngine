#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Layout/AxisLayout.hpp"

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API VerticalLayout : public AxisLayout
	{
		REFLECTED_CLASS(VerticalLayout, AxisLayout)

	public:

		void	ComputeChildrenPositionAndSize() override;
	};
}
