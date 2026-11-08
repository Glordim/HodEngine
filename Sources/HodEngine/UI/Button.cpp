#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Button.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/PointerEvent.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Button, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_GESTURE_TAP_BUTTON);

		AddPropertyT(reflectionDescriptor, &Button::_action, "Action", &Button::SetAction);
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
	Button::ClickEvent& Button::GetClickEvent()
	{
		return _clickEvent;
	}

	/// @brief
	/// @param event
	/// @return
	bool Button::OnPointerEvent(const PointerEvent& event)
	{
		if (event._type == PointerEvent::Type::Click)
		{
			if (IsDisabled() == false)
			{
				_clickEvent.Emit();
				if (_action.Empty() == false && event._canvas != nullptr && GetNode() != nullptr)
				{
					event._canvas->EmitAction(_action, *GetNode());
				}
			}
			return true;
		}
		return Selectable::OnPointerEvent(event);
	}
}
