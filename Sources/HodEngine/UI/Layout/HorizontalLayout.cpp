#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout/HorizontalLayout.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(HorizontalLayout, reflectionDescriptor)
	{
	}

	/// @brief 
	void HorizontalLayout::ComputeChildrenPositionAndSize()
	{
		float min;
		float preferred;
		float flexible;

		CalculateSizesForAxis(Axis::Horizontal, Axis::Horizontal, min, preferred, flexible);
		PlaceAndResizeChildren(Axis::Horizontal, Axis::Horizontal, min, preferred, flexible);

		CalculateSizesForAxis(Axis::Vertical, Axis::Horizontal, min, preferred, flexible);
		PlaceAndResizeChildren(Axis::Vertical, Axis::Horizontal, min, preferred, flexible);
	}
}
