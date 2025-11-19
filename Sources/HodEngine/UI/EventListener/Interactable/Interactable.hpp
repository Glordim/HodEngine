#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/EventListener/EventListener.hpp"
#include "HodEngine/UI/NavigationDirection.hpp"

#include <HodEngine/Core/Reflection/EnumTrait.hpp>
#include <array>

namespace hod::ui
{
	class HOD_UI_API Interactable : public EventListener
	{
		REFLECTED_CLASS(Interactable, EventListener)

	public:

		enum class State : uint8_t
		{
			Normal,
			Highlighted,
			Pressed,
			Disabled,
			DisabledHighlighted,
		};

	public:

		void						SetDisabled(bool disabled);
		bool						IsDisabled() const;

		void						SetAllowFocusWhenDisabled(bool allowFocusWhenDisabled);
		bool						GetAllowFocusWhenDisabled() const;

		void						SetRejectFocus(bool rejectFocus);
		bool						GetRejectFocus() const;

		void						SetPassthrough(bool passthrough);
		bool						GetPassthrough() const;

		Interactable*				GetNavigationDestination(NavigationDirection direction) const;
		void						SetNavigationDestination(NavigationDirection direction, Interactable* destination);

		State						GetState() const;

		virtual bool				OnFocusEvent(const FocusEvent& event);
		virtual bool				OnUnfocusEvent(const UnfocusEvent& event);
		virtual bool				OnNavigateEvent(const NavigationEvent& event);

		bool						OnPointerEnterEvent(const PointerEventEnter& event) override;
		bool						OnPointerExitEvent(const PointerExitEvent& event) override;

		bool						OnPressEvent(const PressEvent& event) override;
		bool						OnReleaseEvent(const ReleaseEvent& event) override;

		bool						OnClickEvent(const ClickEvent& event) override;

		bool						OnCancelEvent(const CancelEvent& event) override;
		bool						OnScrollEvent(const ScrollEvent& event) override;
		bool						OnDragEvent(const DragEvent& event) override;
		bool						OnDragMoveEvent(const DragMoveEvent& event) override;
		bool						OnDropEvent(const DropEvent& event) override;

		bool						CanBeFocus() const;

	private:

		std::array<WeakPtr<Interactable>, EnumTrait::GetCount<NavigationDirection, uint8_t>()>	_navigation;

		bool					_disabled = false;
		bool					_rejectFocus = false;
		bool					_passthrough = false;
		bool					_allowFocusWhenDisabled = false;

		State					_state = State::Normal;
	};
}
