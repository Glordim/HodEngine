#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Control/Control.hpp"
#include "HodEngine/UI/NavigationDirection.hpp"

#include "HodEngine/UI/EventSystem/Event/FocusEvent.hpp"
#include "HodEngine/UI/EventSystem/Event/UnfocusEvent.hpp"

#include <HodEngine/Math/Color.hpp>

#include <HodEngine/Game/WeakComponent.hpp>
#include <HodEngine/Core/Reflection/EnumTrait.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>
#include <HodEngine/Core/Event.hpp>
#include <array>
#include <cstdint>

namespace hod::inline ui
{
	class Texture;
	class Drawable;
	class AnimationGraphPlayer;

	class HOD_UI_API Selectable : public Control
	{
		REFLECTED_CLASS(Selectable, Control)

	public:

		enum class State : uint8_t
		{
			Normal,
			Highlighted,
			Pressed,
			Disabled,
			DisabledHighlighted,
		};

		enum StateTransitionMode
		{
			ColorTint,
			SpriteSwap,
			Animation
		};

		core::Event<const FocusEvent&>	_onFocusEvent;
		core::Event<const UnfocusEvent&> 	_onUnfocusEvent;
		core::Event<const NavigationEvent&> 	_onNavigateEvent;

	public:

		void						OnEnable() override;
		void						OnDisable() override;

		void						SetDisabled(bool disabled);
		bool						IsDisabled() const;

		void						SetAllowFocusWhenDisabled(bool allowFocusWhenDisabled);
		bool						GetAllowFocusWhenDisabled() const;

		void						SetRejectFocus(bool rejectFocus);
		bool						GetRejectFocus() const;

		void						SetPassthrough(bool passthrough);
		bool						GetPassthrough() const;

		Selectable*					GetNavigationDestination(NavigationDirection direction) const;
		void						SetNavigationDestination(NavigationDirection direction, Selectable* destination);

		State						GetState() const;

		StateTransitionMode			GetStateTransitionMode() const;

		virtual bool				OnFocusEvent(const FocusEvent& event);
		virtual bool				OnUnfocusEvent(const UnfocusEvent& event);
		virtual bool				OnNavigateEvent(const NavigationEvent& event);

		bool						OnPointerEnterEvent(const PointerEnterEvent& event) override;
		bool						OnPointerExitEvent(const PointerExitEvent& event) override;

		bool						OnPressEvent(const PressEvent& event) override;
		bool						OnReleaseEvent(const ReleaseEvent& event) override;

		bool						OnClickEvent(const ClickEvent& event) override;

		bool						OnCancelEvent(const CancelEvent& event) override;
		bool						OnScrollEvent(const ScrollEvent& event) override;
		bool						OnDragBeginEvent(const DragBeginEvent& event) override;
		bool						OnDragMoveEvent(const DragMoveEvent& event) override;
		bool						OnDropEvent(const DropEvent& event) override;

		bool						CanBeFocus() const;

		void						Refresh();

		bool						IsPressed() const;
		void						SetPress(bool press);

		bool						IsHighlighted() const;
		bool						IsHovered() const;
		bool						IsFocus() const;

	private:

		void						UpdateInternalState();
		void						UpdateColor(Drawable* drawable);
		void						UpdateSprite(Texture* texture);
		void						UpdateAnimationGraphVariables(AnimationGraphPlayer* animationGraphPlayer);

	private:

		std::array<WeakPtr<Selectable>, static_cast<uint32_t>(NavigationDirection::Count)>	_navigation;

		bool					_disabled = false;
		bool					_rejectFocus = false;
		bool					_passthrough = false;
		bool					_allowFocusWhenDisabled = false;

		//bool					_isHighlighted = false;
		bool					_isHovered = false;
		bool					_isFocus = false;
		bool					_isPressed = false;

		State					_state = State::Normal;
		StateTransitionMode		_stateTransitionMode = StateTransitionMode::ColorTint;

		WeakComponent<Drawable>	_target;

		Color	_normalColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		Color	_highlightedColor = Color(1.0f, 1.0f, 0.9f, 0.38f);
		Color	_pressedColor = Color(1.0f, 0.62f, 0.52f, 0.05f);
		Color	_disabledColor = Color(1.0f, 0.5f, 0.5f, 0.5f);
		Color	_disabledHighlightedColor = Color(1.0f, 0.4f, 0.36f, 0.15f);
	};
}
