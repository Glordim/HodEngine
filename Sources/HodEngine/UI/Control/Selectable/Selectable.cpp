#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Control/Control.hpp"
#include "HodEngine/UI/Control/Selectable/Selectable.hpp"
#include "HodEngine/UI/Drawable/Drawable.hpp"
#include "HodEngine/UI/Drawable/Texture.hpp"

#include "HodEngine/UI/EventSystem/Event/CancelEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/ClickEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/DragMoveEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/DropEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/FocusEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/NavigationEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/PointerEnterEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/PointerExitEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/ScrollEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/UnfocusEvent.hpp"
#include "HodEngine/UI/NavigationDirection.hpp"
#include <HodEngine/Game/Entity.hpp>
#include <cstdint>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Selectable, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Selectable::_target, "_target");

		AddPropertyT(reflectionDescriptor, &Selectable::_normalColor, "_normalColor");
		AddPropertyT(reflectionDescriptor, &Selectable::_highlightedColor, "_highlightedColor");
		AddPropertyT(reflectionDescriptor, &Selectable::_pressedColor, "_pressedColor");
		AddPropertyT(reflectionDescriptor, &Selectable::_disabledColor, "_disabledColor");
		AddPropertyT(reflectionDescriptor, &Selectable::_disabledHighlightedColor, "_disabledHighlightedColor");
	}

	void Selectable::OnEnable()
	{
		Control::OnEnable();

		UpdateInternalState();
	}

	void Selectable::OnDisable()
	{
		Control::OnDisable();

		if (_isFocus && EventSystem::GetInstance()->GetCurrentFocusable() == this)
		{
			EventSystem::GetInstance()->Unfocus(UnfocusEvent::Reason::Disabled);
		}
	}

	/// @brief 
	/// @return 
	Selectable::StateTransitionMode Selectable::GetStateTransitionMode() const
	{
		return _stateTransitionMode;
	}

	/// @brief 
	/// @param interactable 
	void Selectable::SetDisabled(bool disabled)
	{
		_disabled = disabled;
		if (_disabled == false)
		{
			_isPressed = false;
		}

		UpdateInternalState();
	}

	/// @brief 
	/// @return 
	bool Selectable::IsDisabled() const
	{
		return _disabled;
	}

	/// @brief 
	/// @param rejectFocus 
	void Selectable::SetRejectFocus(bool rejectFocus)
	{
		_rejectFocus = rejectFocus;
	}

	/// @brief 
	/// @return 
	bool Selectable::GetRejectFocus() const
	{
		return _rejectFocus;
	}

	/// @brief 
	/// @param passthrough 
	void Selectable::SetPassthrough(bool passthrough)
	{
		_passthrough = passthrough;
	}

	/// @brief 
	/// @return 
	bool Selectable::GetPassthrough() const
	{
		return _passthrough;
	}

	/// @brief 
	/// @param allowFocusWhenDisabled 
	void Selectable::SetAllowFocusWhenDisabled(bool allowFocusWhenDisabled)
	{
		_allowFocusWhenDisabled = allowFocusWhenDisabled;
	}

	/// @brief 
	/// @return 
	bool Selectable::GetAllowFocusWhenDisabled() const
	{
		return _allowFocusWhenDisabled;
	}

	/// @brief 
	/// @param direction 
	/// @return 
	Selectable* Selectable::GetNavigationDestination(NavigationDirection direction) const
	{
		return _navigation[static_cast<uint32_t>(direction)].Get();
	}

	/// @brief 
	/// @param direction 
	/// @param destination 
	void Selectable::SetNavigationDestination(NavigationDirection direction, Selectable* destination)
	{
		_navigation[static_cast<uint32_t>(direction)] = destination;
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnFocusEvent(const FocusEvent& event)
	{
		if (_rejectFocus == true)
		{
			return false;
		}

		_isFocus = true;

		UpdateInternalState();

		_onFocusEvent.Emit(event);

		// TODO move in EventSystem ?
		/*
		NavigationZone* navigationzone = nullptr;
		if (NavigationZone::IsDrivenByNavigationZone(this, navigationzone) == true)
		{
			navigationzone->NotifyChildFocused(this);
		}
		*/
		//
		return true;
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnUnfocusEvent(const UnfocusEvent& event)
	{
		_isFocus = false;
		_isPressed = false;

		UpdateInternalState();

		_onUnfocusEvent.Emit(event);

		// TODO move in EventSystem ?
		/*
		NavigationZone* navigationzone = nullptr;
		if (NavigationZone::IsDrivenByNavigationZone(this, navigationzone) == true)
		{
			navigationzone->NotifyChildUnfocused(this);
		}
		*/
		//

		return true;
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnNavigateEvent(const NavigationEvent& event)
	{
		EventSystem& eventSystem = *EventSystem::GetInstance();
		Selectable* destination = eventSystem.ResolveDestination(this, event._direction);

		if (destination != nullptr)
		{
			eventSystem.Focus(destination, FocusEvent::Reason::Navigation);

			_onNavigateEvent.Emit(event);
			return true;
		}

		return false;
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnPointerEnterEvent(const PointerEnterEvent& event)
	{
		Control::OnPointerEnterEvent(event);

		_isHovered = true;

		EventSystem::GetInstance()->SetLastFocusable(this);

		UpdateInternalState();

		return true;
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnPointerExitEvent(const PointerExitEvent& event)
	{
		Control::OnPointerExitEvent(event);

		_isHovered = false;

		UpdateInternalState();

		return true;
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnPressEvent(const PressEvent& event)
	{
		Control::OnPressEvent(event);

		if (_disabled == false)
		{
			_isPressed = true;

			UpdateInternalState();

			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnReleaseEvent(const ReleaseEvent& event)
	{
		Control::OnReleaseEvent(event);

		if (_disabled == false)
		{
			_isPressed = false;
			UpdateInternalState();
			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnClickEvent(const ClickEvent& event)
	{
		EventSystem::GetInstance()->Focus(this, FocusEvent::Reason::Clicked);

		Control::OnClickEvent(event);

		if (_disabled == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnCancelEvent(const CancelEvent& event)
	{
		Control::OnCancelEvent(event);

		if (_disabled == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnScrollEvent(const ScrollEvent& event)
	{
		Control::OnScrollEvent(event);

		if (_disabled == false)
		{
			return false; // TODO scrollview require false on top element to get this event, rework event consume...
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnDragBeginEvent(const DragBeginEvent& event)
	{
		Control::OnDragBeginEvent(event);

		if (_disabled == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnDragMoveEvent(const DragMoveEvent& event)
	{
		Control::OnDragMoveEvent(event);

		if (_disabled == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief 
	/// @param event 
	/// @return 
	bool Selectable::OnDropEvent(const DropEvent& event)
	{
		Control::OnDropEvent(event);

		if (_disabled == false)
		{
			if (event._reason == DropEvent::Reason::Cancel)
			{
				_isPressed = false;
				UpdateInternalState();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	void Selectable::UpdateInternalState()
	{
		switch (_stateTransitionMode)
		{
			case StateTransitionMode::ColorTint:
			{
				Drawable* drawable = GetOwner()->GetComponent<Drawable>();
				if (drawable != nullptr)
				{
					UpdateColor(drawable);
				}
			}
			break;

			case StateTransitionMode::SpriteSwap:
			{
				Texture* texture = GetOwner()->GetComponent<Texture>();
				if (texture != nullptr)
				{
					UpdateSprite(texture);
				}
			}
			break;

			case StateTransitionMode::Animation:
			{
				AnimationGraphPlayer* animationGraphPlayer = nullptr;// = GetOwner()->GetComponent<AnimationGraphPlayer>();
				if (animationGraphPlayer != nullptr)
				{
					UpdateAnimationGraphVariables(animationGraphPlayer);
				}
			}
			break;
		}
	}

	void Selectable::UpdateColor(Drawable* drawable)
	{
		if (_disabled == true)
		{
			if (_isHovered || _isFocus || _isPressed)
			{
				drawable->SetColor(_disabledHighlightedColor);
			}
			else
			{
				drawable->SetColor(_disabledColor);
			}
		}
		else
		{
			if (_isPressed == true)
			{
				drawable->SetColor(_pressedColor);
			}
			else if (_isHovered || _isFocus)
			{
				drawable->SetColor(_highlightedColor);
			}
			else
			{
				drawable->SetColor(_normalColor);
			}
		}
	}

	void Selectable::UpdateSprite(Texture* /*texture*/)
	{
		/*
		if (_disabled == true)
		{
			if (_isHovered || _isFocus || _isPressed)
			{
				texture->SetSpriteResource(_stateTransitionSpriteDisableHighlight);
			}
			else
			{
				texture->SetSpriteResource(_stateTransitionSpriteDisable);
			}
		}
		else
		{
			if (_isPressed == true)
			{
				texture->SetSpriteResource(_stateTransitionSpritePress);
			}
			else if (_isHovered || _isFocus)
			{
				texture->SetSpriteResource(_stateTransitionSpriteHighlight);
			}
			else
			{
				texture->SetSpriteResource(_stateTransitionSpriteNormal);
			}
		}
			*/
	}

	void Selectable::UpdateAnimationGraphVariables(AnimationGraphPlayer* /*animationGraphPlayer*/)
	{
		/*
		animationGraphPlayer->ResetTriggerByName("DisabledHighlighted");
		animationGraphPlayer->ResetTriggerByName("Disabled");
		animationGraphPlayer->ResetTriggerByName("Pressed");
		animationGraphPlayer->ResetTriggerByName("Highlighted");
		animationGraphPlayer->ResetTriggerByName("Normal");

		if (_isInteractable == false)
		{
			if (_isHovered || _isFocus || _isPressed)
			{
				animationGraphPlayer->SetTriggerByName("DisabledHighlighted");
			}
			else
			{
				animationGraphPlayer->SetTriggerByName("Disabled");
			}
		}
		else
		{
			if (_isPressed == true)
			{
				animationGraphPlayer->SetTriggerByName("Pressed");
			}
			else if (_isHovered || _isFocus)
			{
				animationGraphPlayer->SetTriggerByName("Highlighted");
			}
			else
			{
				animationGraphPlayer->SetTriggerByName("Normal");
			}
		}
		*/
	}

	/// @brief 
	/// @return 
	bool Selectable::IsPressed() const
	{
		return _isPressed;
	}

	/// @brief 
	/// @param press 
	void Selectable::SetPress(bool press)
	{
		_isPressed = press;
	}

	/// @brief 
	/// @return 
	bool Selectable::IsHighlighted() const
	{
		return _isHovered || _isFocus;
	}

	bool Selectable::IsHovered() const
	{
		return _isHovered;
	}

	bool Selectable::IsFocus() const
	{
		return _isFocus;
	}

	/// @brief 
	/// @return 
	bool Selectable::CanBeFocus() const
	{
		return (_disabled == false || _allowFocusWhenDisabled == true) && _rejectFocus == false;
	}

	void Selectable::Refresh()
	{
		Drawable* drawable = _target.Lock();
		if (drawable == nullptr)
		{
			drawable = GetOwner()->GetComponent<Drawable>();
		}
		if (drawable == nullptr)
		{
			return;
		}

		drawable->SetColor(_normalColor); // TODO
	}
}
