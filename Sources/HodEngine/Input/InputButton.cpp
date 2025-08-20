#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputButton.hpp"

#include "HodEngine/Input/Device.hpp"

namespace hod::input
{
	DESCRIBE_REFLECTED_CLASS(InputButton, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	InputButton::InputButton(Identifier identifier, const String& displayName, const StateView& stateView)
	: Input(identifier, displayName, stateView)
	{

	}

	bool InputButton::ReadValue() const
	{
		return GetStateView().ReadBoolValue(GetState());
	}

	bool InputButton::IsPressed() const
	{
		return GetStateView().ReadBoolValue(GetState());
	}

	bool InputButton::IsJustPressed() const
	{
		return IsPressed() == true && GetStateView().ReadBoolValue(GetPreviousState()) == false;
	}

	String InputButton::ToString() const
	{
		return std::format("{}", ReadValue());
	}
}
