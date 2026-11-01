#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Interaction.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>

namespace hod::inline ui2
{
	/// @brief Clickable: tracks its state from the pointer (hovered, pressed) and, on click, emits its click event and its
	/// action on the canvas (see Canvas::GetActionEvent), which is how gameplay is meant to listen to it without knowing
	/// the tree. A disabled button still catches the pointer (nothing under it reacts) but doesn't react itself.
	///
	/// Its look follows its state through property overrides (see PropertyOverride): per state but Normal, values given to
	/// properties of nodes of the prefab (its own shape's color, its label's scale...), eased over the transition duration.
	/// Normal is the prefab as authored: what a state doesn't override goes back to it.
	///
	/// Each state, Normal included, can also play one of the prefab's animation clips when entered (see Node::FindAnimationOwner,
	/// AnimationPlayer), e.g. a bounce when pressed: the clip of the state left is stopped (its properties back to their base
	/// values) unless it is the one entered, which restarts. A clip drives its properties over the overrides touching the same ones.
	class HOD_UI2_API Button : public Interaction
	{
		REFLECTED_CLASS(Button, Interaction)

	public:

		enum class State : uint8_t
		{
			Normal,
			Hovered,
			Pressed, // Pressed with the pointer over it. Pressed then moved out, it goes back to Normal until the pointer comes back
			Disabled,
		};

		using ClickEvent = Event<>;
		using StateChangedEvent = Event<State>;

	public:

		// Name the canvas emits on click (see Canvas::GetActionEvent). Empty: no action, only the click event.
		const String&		GetAction() const;
		void				SetAction(const String& action);

		bool				IsDisabled() const;
		void				SetDisabled(bool disabled);

		State				GetState() const;

		// The overrides applied in state, null for Normal (none). Edited, they apply on the next state change.
		Vector<PropertyOverride>*	GetOverrides(State state);

		// The clip played when entering state, empty for none
		const String&		GetAnimation(State state) const;
		void				SetAnimation(State state, const String& animation);

		float				GetTransitionDuration() const;
		void				SetTransitionDuration(float transitionDuration);

		// How the overrides ease over the transition's duration, when entering any state
		AnimationKey::Easing	GetTransitionEasing() const;
		void				SetTransitionEasing(AnimationKey::Easing transitionEasing);

		ClickEvent&			GetClickEvent();
		StateChangedEvent&	GetStateChangedEvent();

		bool				OnPointerEvent(const PointerEvent& event) override;
		void				Update(float deltaTime) override;
		void				RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers) override;

	private:

		State				ComputeState() const;
		void				RefreshState(float transitionDuration);
		// Stops previous' clip, plays next's
		void				SwitchAnimation(State previous, State next);

	private:

		String				_action;
		bool				_disabled = false;
		float				_transitionDuration = 0.1f; // In seconds
		AnimationKey::Easing	_transitionEasing = AnimationKey::Easing::Linear;

		Vector<PropertyOverride>	_hoveredOverrides;
		Vector<PropertyOverride>	_pressedOverrides;
		Vector<PropertyOverride>	_disabledOverrides;

		String				_normalAnimation;
		String				_hoveredAnimation;
		String				_pressedAnimation;
		String				_disabledAnimation;

		PropertyOverrideTransition	_transition;
		bool				_stateApplied = false; // Whether the state was computed once (a button loaded disabled starts so, without transition)

		bool				_hovered = false;
		bool				_pressed = false;
		State				_state = State::Normal;

		ClickEvent			_clickEvent;
		StateChangedEvent	_stateChangedEvent;
	};
}
