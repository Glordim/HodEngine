#pragma once
#include "HodEngine/UI/Export.hpp"
#include "HodEngine/UI/Selectable.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>

namespace hod::inline ui
{
	/// @brief Clickable: on click, emits its click event and its action on the canvas (see Canvas::GetActionEvent), which is how
	/// gameplay is meant to listen to it without knowing the tree. Its look follows its state (see Selectable).
	class HOD_UI_API Button : public Selectable
	{
		REFLECTED_CLASS(Button, Selectable)

	public:

		using ClickEvent = Event<>;

	public:

		// Name the canvas emits on click (see Canvas::GetActionEvent). Empty: no action, only the click event.
		const String&		GetAction() const;
		void				SetAction(const String& action);

		ClickEvent&			GetClickEvent();

		bool				OnPointerEvent(const PointerEvent& event) override;

	private:

		String				_action;

		ClickEvent			_clickEvent;
	};
}
