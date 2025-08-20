#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Input.hpp"

namespace hod::input
{
	DESCRIBE_REFLECTED_CLASS(Input, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	Input::Input(Identifier identifier, const String& displayName, const StateView& stateView)
	: _identifier(identifier)
	, _displayName(displayName)
	, _stateView(stateView)
	{

	}

	void Input::SetStatePtr(const State* state, const State* previousState)
	{
		_state = state;
		_previousState = previousState;
	}

	/// @brief 
	/// @return 
	Identifier Input::GetIdentifier() const
	{
		return _identifier;
	}

	const String& Input::GetDisplayName() const
	{
		return _displayName;
	}

	const StateView& Input::GetStateView() const
	{
		return _stateView;
	}

	const State* Input::GetState() const
	{
		return _state;
	}

	const State* Input::GetPreviousState() const
	{
		return _previousState;
	}
}
