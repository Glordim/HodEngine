#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Selectable.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>

namespace hod::inline ui2
{
	/// @brief On or off, switched by a click (a checkbox, a switch, a tab). Toggles of the same group in a prefab are exclusive
	/// (radio buttons): switching one on switches the others off, and clicking the one on leaves it on.
	///
	/// On top of the states (see Selectable), two looks: Off, the prefab as authored, and On, its overrides (a check mark's
	/// scale, a switch's knob offset...) layered under the state's, each with the clip played when switched to it. Switching
	/// emits the value changed event and the action (see Canvas::GetActionEvent: gameplay reads IsOn() from the node).
	class HOD_UI2_API Toggle : public Selectable
	{
		REFLECTED_CLASS(Toggle, Selectable)

	public:

		using ValueChangedEvent = Event<bool>;

		// Looks after the states (see Selectable::GetLookCount)
		static constexpr uint32_t OffLook = StateCount;
		static constexpr uint32_t OnLook = StateCount + 1;

	public:

		bool				IsOn() const;
		// Switches it (the other toggles of its group off when on), with the transition, the clip and the value changed event,
		// not the action (emitted for the pointer's clicks only)
		void				SetOn(bool on);

		// Toggles of the same group (not empty) under the same prefab are exclusive
		const String&		GetGroup() const;
		void				SetGroup(const String& group);

		// Name the canvas emits when a click switches it (see Canvas::GetActionEvent). Empty: no action, only the event.
		const String&		GetAction() const;
		void				SetAction(const String& action);

		Vector<PropertyOverride>&	GetOnOverrides();

		ValueChangedEvent&	GetValueChangedEvent();

		uint32_t			GetLookCount() const override;
		const char*			GetLookName(uint32_t look) const override;
		Vector<PropertyOverride>*	GetLookOverrides(uint32_t look) override;
		String*				GetLookAnimation(uint32_t look) override;

		bool				OnPointerEvent(const PointerEvent& event) override;

	protected:

		void				CollectActiveLooks(Vector<uint32_t>& outLooks) const override;

	private:

		// The reflected setter: the value as authored, switched without transition, clip nor event
		void				SetOnProperty(bool on);
		void				Switch(bool on, float transitionDuration, bool playAnimation);
		// The other toggles of its group in its prefab
		void				SwitchGroupOff();

	private:

		bool				_on = false;
		String				_group;
		String				_action;

		Vector<PropertyOverride>	_onOverrides;
		String				_offAnimation;
		String				_onAnimation;

		ValueChangedEvent	_valueChangedEvent;
	};
}
