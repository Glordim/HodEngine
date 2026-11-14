#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Toggle.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/PointerEvent.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Toggle, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_TOGGLE_SWITCH);

		HOD_PROPERTY(_on, &Toggle::SetOnProperty);
		HOD_PROPERTY(_group, &Toggle::SetGroup);
		HOD_PROPERTY(_action, &Toggle::SetAction);

		// Edited by the UI prefab editor's own drawer, as the states' (see Selectable)
		HOD_PROPERTY(_onOverrides)->AddTrait<ReflectionTraitHide>();
		HOD_PROPERTY(_offAnimation)->AddTrait<ReflectionTraitHide>();
		HOD_PROPERTY(_onAnimation)->AddTrait<ReflectionTraitHide>();
	}

	/// @brief
	/// @return
	bool Toggle::IsOn() const
	{
		return _on;
	}

	/// @brief
	/// @param on
	void Toggle::SetOn(bool on)
	{
		Switch(on, GetTransitionDuration(), true);
	}

	/// @brief
	/// @param on
	void Toggle::SetOnProperty(bool on)
	{
		if (_on != on)
		{
			_on = on;
			RefreshState(0.0f, true);
		}
	}

	/// @brief
	/// @return
	const String& Toggle::GetGroup() const
	{
		return _group;
	}

	/// @brief
	/// @param group
	void Toggle::SetGroup(const String& group)
	{
		_group = group;
	}

	/// @brief
	/// @return
	const String& Toggle::GetAction() const
	{
		return _action;
	}

	/// @brief
	/// @param action
	void Toggle::SetAction(const String& action)
	{
		_action = action;
	}

	/// @brief
	/// @return
	Vector<PropertyOverride>& Toggle::GetOnOverrides()
	{
		return _onOverrides;
	}

	/// @brief
	/// @return
	Toggle::ValueChangedEvent& Toggle::GetValueChangedEvent()
	{
		return _valueChangedEvent;
	}

	/// @brief
	/// @return
	uint32_t Toggle::GetLookCount() const
	{
		return StateCount + 2;
	}

	/// @brief
	/// @param look
	/// @return
	const char* Toggle::GetLookName(uint32_t look) const
	{
		switch (look)
		{
			case OffLook: return "Off";
			case OnLook: return "On";
			default: return Selectable::GetLookName(look);
		}
	}

	/// @brief
	/// @param look
	/// @return
	Vector<PropertyOverride>* Toggle::GetLookOverrides(uint32_t look)
	{
		switch (look)
		{
			case OffLook: return nullptr; // the prefab as authored
			case OnLook: return &_onOverrides;
			default: return Selectable::GetLookOverrides(look);
		}
	}

	/// @brief
	/// @param look
	/// @return
	String* Toggle::GetLookAnimation(uint32_t look)
	{
		switch (look)
		{
			case OffLook: return &_offAnimation;
			case OnLook: return &_onAnimation;
			default: return Selectable::GetLookAnimation(look);
		}
	}

	/// @brief
	/// @param outLooks
	void Toggle::CollectActiveLooks(Vector<uint32_t>& outLooks) const
	{
		if (_on)
		{
			outLooks.PushBack(OnLook);
		}
		Selectable::CollectActiveLooks(outLooks);
	}

	/// @brief
	/// @param event
	/// @return
	bool Toggle::OnPointerEvent(const PointerEvent& event)
	{
		if (event._type == PointerEvent::Type::Click)
		{
			// In a group, the one on stays on
			if (IsDisabled() == false && (_on == false || _group.Empty()))
			{
				Switch(_on == false, GetTransitionDuration(), true);
				if (_action.Empty() == false && event._canvas != nullptr && GetNode() != nullptr)
				{
					event._canvas->EmitAction(_action, *GetNode());
				}
			}
			return true;
		}
		return Selectable::OnPointerEvent(event);
	}

	/// @brief
	/// @param on
	/// @param transitionDuration
	/// @param playAnimation
	void Toggle::Switch(bool on, float transitionDuration, bool playAnimation)
	{
		if (_on == on)
		{
			return;
		}
		_on = on;

		if (_on)
		{
			SwitchGroupOff();
		}
		RefreshState(transitionDuration, true);
		if (playAnimation && GetNode() != nullptr)
		{
			SwitchAnimation(_on ? _offAnimation : _onAnimation, _on ? _onAnimation : _offAnimation);
		}
		_valueChangedEvent.Emit(_on);
	}

	/// @brief
	void Toggle::SwitchGroupOff()
	{
		if (_group.Empty() || GetNode() == nullptr)
		{
			return;
		}

		// The nodes of its prefab: its scope, without the nested instances' content (their own prefab's)
		Node*         scopeRoot = GetNode()->GetScopeRoot();
		Vector<Node*> nodes = {scopeRoot};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			Node* node = nodes[index];
			if (Toggle* toggle = node->GetInteraction<Toggle>(); toggle != nullptr && toggle != this && toggle->_group == _group)
			{
				toggle->Switch(false, toggle->GetTransitionDuration(), true);
			}
			if (node != scopeRoot && node->GetPrefabInstance() != nullptr)
			{
				continue;
			}
			for (Node* child : node->GetChildren())
			{
				nodes.PushBack(child);
			}
		}
	}
}
