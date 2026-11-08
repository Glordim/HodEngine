#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Selectable.hpp"
#include "HodEngine/UI/AnimationPlayer.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/PointerEvent.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Selectable, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Selectable::_disabled, "Disabled", &Selectable::SetDisabled);
		AddPropertyT(reflectionDescriptor, &Selectable::_transitionDuration, "TransitionDuration", &Selectable::SetTransitionDuration);
		AddPropertyT(reflectionDescriptor, &Selectable::_transitionEasing, "TransitionEasing", &Selectable::SetTransitionEasing);

		// Edited by the UI prefab editor's own drawer (target node and property pickers), not the generic one
		AddPropertyT(reflectionDescriptor, &Selectable::_hoveredOverrides, "HoveredOverrides")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Selectable::_pressedOverrides, "PressedOverrides")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Selectable::_disabledOverrides, "DisabledOverrides")->AddTrait<ReflectionTraitHide>();

		// Picked among the prefab's clips by the UI prefab editor's drawer
		AddPropertyT(reflectionDescriptor, &Selectable::_normalAnimation, "NormalAnimation")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Selectable::_hoveredAnimation, "HoveredAnimation")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Selectable::_pressedAnimation, "PressedAnimation")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Selectable::_disabledAnimation, "DisabledAnimation")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief
	/// @return
	bool Selectable::IsDisabled() const
	{
		return _disabled;
	}

	/// @brief
	/// @param disabled
	void Selectable::SetDisabled(bool disabled)
	{
		if (_disabled != disabled)
		{
			_disabled = disabled;
			if (_disabled)
			{
				_pressed = false; // A press in progress won't give a click
			}
			RefreshState(_transitionDuration);
		}
	}

	/// @brief
	/// @return
	Selectable::State Selectable::GetState() const
	{
		return _state;
	}

	/// @brief
	/// @param state
	/// @return
	Vector<PropertyOverride>* Selectable::GetOverrides(State state)
	{
		switch (state)
		{
			case State::Normal: return nullptr;
			case State::Hovered: return &_hoveredOverrides;
			case State::Pressed: return &_pressedOverrides;
			case State::Disabled: return &_disabledOverrides;
		}
		return nullptr;
	}

	/// @brief
	/// @param state
	/// @return
	const String& Selectable::GetAnimation(State state) const
	{
		switch (state)
		{
			case State::Normal: return _normalAnimation;
			case State::Hovered: return _hoveredAnimation;
			case State::Pressed: return _pressedAnimation;
			case State::Disabled: return _disabledAnimation;
		}
		return _normalAnimation;
	}

	/// @brief
	/// @param state
	/// @param animation
	void Selectable::SetAnimation(State state, const String& animation)
	{
		const_cast<String&>(GetAnimation(state)) = animation;
	}

	/// @brief
	/// @return
	float Selectable::GetTransitionDuration() const
	{
		return _transitionDuration;
	}

	/// @brief
	/// @param transitionDuration
	void Selectable::SetTransitionDuration(float transitionDuration)
	{
		_transitionDuration = transitionDuration;
	}

	/// @brief
	/// @return
	AnimationKey::Easing Selectable::GetTransitionEasing() const
	{
		return _transitionEasing;
	}

	/// @brief
	/// @param transitionEasing
	void Selectable::SetTransitionEasing(AnimationKey::Easing transitionEasing)
	{
		_transitionEasing = transitionEasing;
	}

	/// @brief
	/// @return
	uint32_t Selectable::GetLookCount() const
	{
		return StateCount;
	}

	/// @brief
	/// @param look
	/// @return
	const char* Selectable::GetLookName(uint32_t look) const
	{
		constexpr const char* stateNames[StateCount] = {"Normal", "Hovered", "Pressed", "Disabled"};
		return look < StateCount ? stateNames[look] : "";
	}

	/// @brief
	/// @param look
	/// @return
	Vector<PropertyOverride>* Selectable::GetLookOverrides(uint32_t look)
	{
		return look < StateCount ? GetOverrides((State)look) : nullptr;
	}

	/// @brief
	/// @param look
	/// @return
	String* Selectable::GetLookAnimation(uint32_t look)
	{
		return look < StateCount ? &const_cast<String&>(GetAnimation((State)look)) : nullptr;
	}

	/// @brief
	/// @return
	Selectable::StateChangedEvent& Selectable::GetStateChangedEvent()
	{
		return _stateChangedEvent;
	}

	/// @brief
	/// @param event
	/// @return
	bool Selectable::OnPointerEvent(const PointerEvent& event)
	{
		switch (event._type)
		{
			case PointerEvent::Type::Enter: _hovered = true; break;
			case PointerEvent::Type::Exit: _hovered = false; break;

			case PointerEvent::Type::Press:
			{
				// Handled even when disabled: the press must not reach what is under it
				if (_disabled == false)
				{
					_pressed = true;
				}
			}
			break;

			case PointerEvent::Type::Release: _pressed = false; break;

			default: return false;
		}

		RefreshState(_transitionDuration);
		return true;
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void Selectable::RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		for (uint32_t look = 0; look < GetLookCount(); ++look)
		{
			Vector<PropertyOverride>* overrides = GetLookOverrides(look);
			if (overrides == nullptr)
			{
				continue;
			}
			for (uint32_t index = 0; index < overrides->Size();)
			{
				PropertyOverride& propertyOverride = (*overrides)[index];
				auto              it = oldToNew.find(propertyOverride._target);
				if (it != oldToNew.end())
				{
					propertyOverride._target = it->second;
				}
				else if (keepOthers == false)
				{
					overrides->Erase(index);
					continue;
				}
				++index;
			}
		}
	}

	/// @brief
	/// @param deltaTime
	void Selectable::Update(float deltaTime)
	{
		if (_stateApplied == false)
		{
			RefreshState(0.0f, true);
		}

		if (GetNode() != nullptr)
		{
			_transition.Update(*GetNode(), deltaTime);
		}
	}

	/// @brief
	/// @param outLooks
	void Selectable::CollectActiveLooks(Vector<uint32_t>& outLooks) const
	{
		outLooks.PushBack((uint32_t)_state);
	}

	/// @brief
	/// @return
	bool Selectable::IsHovered() const
	{
		return _hovered;
	}

	/// @brief
	/// @return
	bool Selectable::IsPressed() const
	{
		return _pressed;
	}

	/// @brief
	/// @return
	Selectable::State Selectable::ComputeState() const
	{
		State state = State::Normal;
		if (_disabled)
		{
			state = State::Disabled;
		}
		else if (_pressed && _hovered)
		{
			state = State::Pressed;
		}
		else if (_hovered && _pressed == false)
		{
			state = State::Hovered;
		}
		return state;
	}

	/// @brief
	/// @param previous
	/// @param next
	void Selectable::SwitchAnimation(const String& previous, const String& next)
	{
		std::string_view previousName(previous.CStr(), previous.Length());
		std::string_view nextName(next.CStr(), next.Length());

		// Stopped then the next one started in the same frame: no frame of the base values in between
		if (previousName.empty() == false && previousName != nextName)
		{
			if (Node* owner = GetNode()->FindAnimationOwner(previousName))
			{
				owner->GetAnimationPlayer().Stop(previousName);
			}
		}
		if (nextName.empty() == false)
		{
			if (Node* owner = GetNode()->FindAnimationOwner(nextName))
			{
				owner->GetAnimationPlayer().Play(nextName);
			}
		}
	}

	/// @brief Switches to the state the pointer and Disabled give, if it changed, easing to the active looks' overrides.
	/// @param transitionDuration
	/// @param force
	void Selectable::RefreshState(float transitionDuration, bool force)
	{
		// Not attached yet (e.g. Disabled set while deserializing): the first Update computes it
		if (GetNode() == nullptr)
		{
			return;
		}
		_stateApplied = true;

		State state = ComputeState();
		if (_state == state && force == false)
		{
			return;
		}
		State previous = _state;
		_state = state;

		// Every look's overrides get a track (see PropertyOverrideTransition), the active ones layered: a later look wins
		Vector<const Vector<PropertyOverride>*> allSets;
		for (uint32_t look = 0; look < GetLookCount(); ++look)
		{
			if (const Vector<PropertyOverride>* overrides = GetLookOverrides(look))
			{
				allSets.PushBack(overrides);
			}
		}
		Vector<uint32_t> activeLooks;
		CollectActiveLooks(activeLooks);
		Vector<PropertyOverride> active;
		for (uint32_t look : activeLooks)
		{
			const Vector<PropertyOverride>* overrides = GetLookOverrides(look);
			if (overrides == nullptr)
			{
				continue;
			}
			for (const PropertyOverride& propertyOverride : *overrides)
			{
				bool replaced = false;
				for (PropertyOverride& existing : active)
				{
					if (existing._target == propertyOverride._target && existing._property == propertyOverride._property)
					{
						existing = propertyOverride;
						replaced = true;
						break;
					}
				}
				if (replaced == false)
				{
					active.PushBack(propertyOverride);
				}
			}
		}
		_transition.Start(*GetNode(), &active, allSets, transitionDuration, _transitionEasing);

		if (previous != _state)
		{
			SwitchAnimation(GetAnimation(previous), GetAnimation(_state));
			_stateChangedEvent.Emit(_state);
		}
	}
}
