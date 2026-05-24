#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Control/Control.hpp"

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Control, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	bool Control::OnPointerEnterEvent(const PointerEnterEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnPointerExitEvent(const PointerExitEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnPressEvent(const PressEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnReleaseEvent(const ReleaseEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnClickEvent(const ClickEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnCancelEvent(const CancelEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnScrollEvent(const ScrollEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnDragBeginEvent(const DragBeginEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnDragMoveEvent(const DragMoveEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnDropEvent(const DropEvent& /*event*/)
	{
		return false;
	}

	bool Control::OnNavigationEvent(const NavigationEvent& /*event*/)
	{
		return false;
	}
}

