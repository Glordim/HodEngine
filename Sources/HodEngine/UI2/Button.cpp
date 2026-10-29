#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/Button.hpp"
#include "HodEngine/UI2/Canvas.hpp"
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

		// Edited by the UI prefab editor's own drawer (target node and property pickers), not the generic one
		AddPropertyT(reflectionDescriptor, &Button::_hoveredOverrides, "HoveredOverrides")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_pressedOverrides, "PressedOverrides")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Button::_disabledOverrides, "DisabledOverrides")->AddTrait<ReflectionTraitHide>();
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
		_state = state;

		Vector<const Vector<PropertyOverride>*> allSets = {&_hoveredOverrides, &_pressedOverrides, &_disabledOverrides};
		_transition.Start(*GetNode(), GetOverrides(_state), allSets, transitionDuration);

		_stateChangedEvent.Emit(_state);
	}
}
