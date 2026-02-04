#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputAxis.hpp"

namespace hod::input
{
	DESCRIBE_REFLECTED_CLASS(InputAxis, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	InputAxis::InputAxis(Identifier identifier, const String& displayName, const StateView& stateView)
	: Input(identifier, displayName, stateView)
	{
	}

	float InputAxis::ReadValue() const
	{
		return GetStateView().ReadFloatValue(GetState());
	}

	String InputAxis::ToString() const
	{
		return fmt::format("{}", ReadValue()).c_str();
	}
}
