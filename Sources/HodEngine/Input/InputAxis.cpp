#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputAxis.hpp"

#include "HodEngine/Input/Device.hpp"

namespace hod::input
{
	DESCRIBE_REFLECTED_CLASS(InputAxis, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	InputAxis::InputAxis(Identifier identifier, const String& displayName, const StateView& stateView, const State* state, const State* previousState)
	: Input(identifier, displayName, stateView, state, previousState)
	{

	}

	float InputAxis::ReadValue() const
	{
		return GetStateView().ReadFloatValue(GetState());
	}

	String InputAxis::ToString() const
	{
		return std::format("{}", ReadValue());
	}
}
