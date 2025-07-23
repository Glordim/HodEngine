#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>

namespace hod::ui
{
	struct PointerEnterEvent;
	struct PointerExitEvent;
	struct PressEvent;
	struct ReleaseEvent;
	struct ClickEvent;
	struct CancelEvent;
	struct ScrollEvent;
	struct DragBeginEvent;
	struct DragMoveEvent;
	struct DropEvent;
	struct NavigationEvent;

	class HOD_UI_API EventListener : public game::Component
	{
		REFLECTED_CLASS(EventListener, game::Component)

	public:

		virtual bool			OnPointerEnterEvent(const PointerEnterEvent& event);
		virtual bool			OnPointerExitEvent(const PointerExitEvent& event);
		virtual bool			OnPressEvent(const PressEvent& event);
		virtual bool			OnReleaseEvent(const ReleaseEvent& event);
		virtual bool			OnClickEvent(const ClickEvent& event);
		virtual bool			OnCancelEvent(const CancelEvent& event);
		virtual bool			OnScrollEvent(const ScrollEvent& event);
		virtual bool			OnDragBeginEvent(const DragBeginEvent& event);
		virtual bool			OnDragMoveEvent(const DragMoveEvent& event);
		virtual bool			OnDropEvent(const DropEvent& event);
		virtual bool			OnNavigationEvent(const NavigationEvent& event);
	};
}

