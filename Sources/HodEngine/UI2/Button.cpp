#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/Button.hpp"
#include "HodEngine/UI2/AnimationPlayer.hpp"
#include "HodEngine/UI2/Canvas.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/PointerEvent.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(Button, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_GESTURE_TAP_BUTTON);

		AddPropertyT(reflectionDescriptor, &Button::_action, "Action", &Button::SetAction);
		AddPropertyT(reflectionDescriptor, &Button::_disabled, "Disabled", &Button::SetDisabled);
		AddPropertyT(reflectionDescriptor, &Button::_transitionDuration, "TransitionDuration", &Button::SetTransitionDuration);
		AddPropertyT(reflectionDescriptor, &Button::_transitionEasing, "TransitionEasing", &Button::SetTransitionEasing);

		// Edited by the UI prefab editor's own drawer (target node and property pickers), not the generic one
		AddPropertyT(reflectionDescriptor, &Button::_hoveredOverrides, "HoveredOverrides")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_pressedOverrides, "PressedOverrides")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_disabledOverrides, "DisabledOverrides")->AddTrait<ReflectionTraitHide>();

		// Picked among the prefab's clips by the UI prefab editor's drawer
		AddPropertyT(reflectionDescriptor, &Button::_normalAnimation, "NormalAnimation")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_hoveredAnimation, "HoveredAnimation")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_pressedAnimation, "PressedAnimation")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_disabledAnimation, "DisabledAnimation")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief
	/// @return
	const String& Button::GetAction() const
	{
		return _action;
	}

	/// @brief
	/// @param action
	void Button::SetAction(const String& action)
	{
		_action = action;
	}

	/// @brief
	/// @return
	bool Button::IsDisabled() const
	{
		return _disabled;
	}

	/// @brief
	/// @param disabled
	void Button::SetDisabled(bool disabled)
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
	/// @param state
	/// @return
	Vector<PropertyOverride>* Button::GetOverrides(State state)
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
	const String& Button::GetAnimation(State state) const
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
	void Button::SetAnimation(State state, const String& animation)
	{
		const_cast<String&>(GetAnimation(state)) = animation;
	}

	/// @brief
	/// @return
	float Button::GetTransitionDuration() const
	{
		return _transitionDuration;
	}

	/// @brief
	/// @param transitionDuration
	void Button::SetTransitionDuration(float transitionDuration)
	{
		_transitionDuration = transitionDuration;
	}

	/// @brief
	/// @return
	AnimationKey::Easing Button::GetTransitionEasing() const
	{
		return _transitionEasing;
	}

	/// @brief
	/// @param transitionEasing
	void Button::SetTransitionEasing(AnimationKey::Easing transitionEasing)
	{
		_transitionEasing = transitionEasing;
	}

	/// @brief
	/// @return
	Button::State Button::GetState() const
	{
		return _state;
	}

	/// @brief
	/// @return
	Button::ClickEvent& Button::GetClickEvent()
	{
		return _clickEvent;
	}

	/// @brief
	/// @return
	Button::StateChangedEvent& Button::GetStateChangedEvent()
	{
		return _stateChangedEvent;
	}

	/// @brief
	/// @param event
	/// @return
	bool Button::OnPointerEvent(const PointerEvent& event)
	{
		switch (event._type)
		{
			case PointerEvent::Type::Enter: _hovered = true; break;
			case PointerEvent::Type::Exit: _hovered = false; break;

			case PointerEvent::Type::Press:
			{
				// Handled even when disabled: the press must not reach what is under the button
				if (_disabled == false)
				{
					_pressed = true;
				}
			}
			break;

			case PointerEvent::Type::Release: _pressed = false; break;

			case PointerEvent::Type::Click:
			{
				if (_disabled == false)
				{
					_clickEvent.Emit();
					if (_action.Empty() == false && event._canvas != nullptr && GetNode() != nullptr)
					{
						event._canvas->EmitAction(_action, *GetNode());
					}
				}
			}
			break;
		}

		RefreshState(_transitionDuration);
		return true;
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void Button::RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		for (Vector<PropertyOverride>* overrides : {&_hoveredOverrides, &_pressedOverrides, &_disabledOverrides})
		{
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
	void Button::Update(float deltaTime)
	{
		if (_stateApplied == false)
		{
			RefreshState(0.0f);
		}

		if (GetNode() != nullptr)
		{
			_transition.Update(*GetNode(), deltaTime);
		}
	}

	/// @brief
	/// @return
	Button::State Button::ComputeState() const
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
	void Button::SwitchAnimation(State previous, State next)
	{
		const String& previousAnimation = GetAnimation(previous);
		const String& nextAnimation = GetAnimation(next);
		std::string_view previousName(previousAnimation.CStr(), previousAnimation.Length());
		std::string_view nextName(nextAnimation.CStr(), nextAnimation.Length());

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

	/// @brief Switches to the state the pointer and Disabled give, if it changed, easing to its overrides.
	/// @param transitionDuration
	void Button::RefreshState(float transitionDuration)
	{
		// Not attached yet (e.g. Disabled set while deserializing): the first Update computes it
		if (GetNode() == nullptr)
		{
			return;
		}
		_stateApplied = true;

		State state = ComputeState();
		if (_state == state)
		{
			return;
		}
		State previous = _state;
		_state = state;

		Vector<const Vector<PropertyOverride>*> allSets = {&_hoveredOverrides, &_pressedOverrides, &_disabledOverrides};
		_transition.Start(*GetNode(), GetOverrides(_state), allSets, transitionDuration, _transitionEasing);
		SwitchAnimation(previous, _state);

		_stateChangedEvent.Emit(_state);
	}
}
