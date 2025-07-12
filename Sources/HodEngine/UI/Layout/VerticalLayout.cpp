#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout/VerticalLayout.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(VerticalLayout, reflectionDescriptor)
	{
	}

	/// @brief 
	void VerticalLayout::ComputeChildrenPositionAndSize()
	{
		float min;
		float preferred;
		float flexible;

		CalculateSizesForAxis(Axis::Horizontal, Axis::Vertical, min, preferred, flexible);
		PlaceAndResizeChildren(Axis::Horizontal, Axis::Vertical, min, preferred, flexible);

		CalculateSizesForAxis(Axis::Vertical, Axis::Vertical, min, preferred, flexible);
		PlaceAndResizeChildren(Axis::Vertical, Axis::Vertical, min, preferred, flexible);
	}
}
