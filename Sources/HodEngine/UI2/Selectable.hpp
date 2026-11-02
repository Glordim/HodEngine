#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Interaction.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline ui2
{
	/// @brief Base of the Interactions having a state from the pointer (Button, Toggle, Slider): hovered, pressed, disabled. A
	/// disabled one still catches the pointer (nothing under it reacts) but doesn't react itself.
	///
	/// Its look follows its state through property overrides (see PropertyOverride): per state but Normal, values given to
	/// properties of nodes of the prefab (its own shape's color, its label's scale...), eased over the transition duration.
	/// Normal is the prefab as authored: what a state doesn't override goes back to it.
	///
	/// Each state, Normal included, can also play one of the prefab's animation clips when entered (see Node::FindAnimationOwner,
	/// AnimationPlayer), e.g. a bounce when pressed: the clip of the state left is stopped (its properties back to their base
	/// values) unless it is the one entered, which restarts. A clip drives its properties over the overrides touching the same ones.
	///
	/// The states are its first looks: a type can add its own (Toggle's Off and On), each with its overrides (or none, as Normal)
	/// and its clip, layered under the state's (see CollectActiveLooks). What the editor shows and records, by look index.
	class HOD_UI2_API Selectable : public Interaction
	{
		REFLECTED_CLASS(Selectable, Interaction)

	public:

		enum class State : uint8_t
		{
			Normal,
			Hovered,
			Pressed, // Pressed with the pointer over it. Pressed then moved out, it goes back to Normal until the pointer comes back
			Disabled,
		};
		static constexpr uint32_t StateCount = 4;

		using StateChangedEvent = Event<State>;

	public:

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

		// How the overrides ease over the transition's duration, when entering any look
		AnimationKey::Easing	GetTransitionEasing() const;
		void				SetTransitionEasing(AnimationKey::Easing transitionEasing);

		// The looks: the states (by their value), then the type's own
		virtual uint32_t	GetLookCount() const;
		virtual const char*	GetLookName(uint32_t look) const;
		// Its overrides, null when it has none (Normal: the prefab as authored)
		virtual Vector<PropertyOverride>*	GetLookOverrides(uint32_t look);
		// The clip played when it is entered (empty for none), null for no look
		virtual String*		GetLookAnimation(uint32_t look);

		StateChangedEvent&	GetStateChangedEvent();

		// Enter/Exit/Press/Release (the state), handled; the other events aren't
		bool				OnPointerEvent(const PointerEvent& event) override;
		void				Update(float deltaTime) override;
		void				RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers) override;

	protected:

		// The looks in effect, bottom to top (the overrides of the later ones win): the state alone by default
		virtual void		CollectActiveLooks(Vector<uint32_t>& outLooks) const;

		// Eases to the active looks' overrides if the state changed (or force, e.g. a Toggle switched), playing the clip of the
		// state entered
		void				RefreshState(float transitionDuration, bool force = false);
		// Stops previous' clip (unless it is next's, which restarts), plays next's
		void				SwitchAnimation(const String& previous, const String& next);

		// Whether the pointer is over it / pressed it
		bool				IsHovered() const;
		bool				IsPressed() const;

	private:

		State				ComputeState() const;

	private:

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
		bool				_stateApplied = false; // Whether the state was computed once (one loaded disabled starts so, without transition)

		bool				_hovered = false;
		bool				_pressed = false;
		State				_state = State::Normal;

		StateChangedEvent	_stateChangedEvent;
	};
}
