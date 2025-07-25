#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/EventListener/EventListener.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(EventListener, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	bool EventListener::OnPointerEnterEvent(const PointerEnterEvent& event)
	{
		return false;
	}

	bool EventListener::OnPointerExitEvent(const PointerExitEvent& event)
	{
		return false;
	}

	bool EventListener::OnPressEvent(const PressEvent& event)
	{
		return false;
	}

	bool EventListener::OnReleaseEvent(const ReleaseEvent& event)
	{
		return false;
	}

	bool EventListener::OnClickEvent(const ClickEvent& event)
	{
		return false;
	}

	bool EventListener::OnCancelEvent(const CancelEvent& event)
	{
		return false;
	}

	bool EventListener::OnScrollEvent(const ScrollEvent& event)
	{
		return false;
	}

	bool EventListener::OnDragBeginEvent(const DragBeginEvent& event)
	{
		return false;
	}

	bool EventListener::OnDragMoveEvent(const DragMoveEvent& event)
	{
		return false;
	}

	bool EventListener::OnDropEvent(const DropEvent& event)
	{
		return false;
	}

	bool EventListener::OnNavigationEvent(const NavigationEvent& event)
	{
		return false;
	}
}

