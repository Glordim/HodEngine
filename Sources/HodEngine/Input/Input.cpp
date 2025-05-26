#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Input.hpp"

#include <cmath>

namespace hod::input
{
	/// @brief 
	/// @param inputId 
	Input::Input(InputId inputId)
		: _inputId(inputId)
	{
	}

	/// @brief 
	/// @param newValue 
	void Input::SetValue(float newValue)
	{
		if (_value != newValue)
		{
			if (_value == 0.0f)
			{
				_repeatCount = 0;
				_elapsedTime = 0.0f;

				_flags = Flag::Pressed | Flag::JustPressed;
			}
			else if (newValue == 0.0f)
			{
				_flags = Flag::Released | Flag::JustReleased;
			}

			_value = newValue;
		}
	}

	/// @brief 
	void Input::ClearInputFlags()
	{
		_flags &= ~(Flag::JustPressed | Flag::JustReleased | Flag::JustRepeat);
	}

	/// @brief 
	void Input::UpdateInputFlags()
	{
		if ((_flags & Flag::Pressed) == Flag::Pressed)
		{
			_elapsedTime += 0.016f; // TODO SystemTime
			if (_repeatCount > 0 && _elapsedTime > Input::RepeatTimestep)
			{
				_elapsedTime -= Input::RepeatTimestep;
				_flags |= Flag::JustRepeat;
				++_repeatCount;
			}
			else if (_repeatCount == 0 && _elapsedTime > Input::RepeatThreshold)
			{
				_elapsedTime -= Input::RepeatThreshold;
				_flags |= Flag::JustRepeat;
				++_repeatCount;
			}
		}
	}

	/// @brief 
	/// @return 
	InputId Input::GetInputId() const
	{
		return _inputId;
	}

	/// @brief 
	/// @return 
	float Input::GetValue() const
	{
		return _value;
	}

	/// @brief 
	/// @return 
	uint8_t Input::GetFlags() const
	{
		return _flags;
	}

	/// @brief 
	/// @return 
	Input::State Input::GetState() const
	{
		State state(_value, _flags, _inputId);
		return state;
	}

	/// @brief 
	/// @param value 
	/// @param flags 
	/// @param inputId 
	Input::State::State(float value, uint8_t flags, InputId inputId)
		: _value(value)
		, _flags(flags)
		, _inputId(inputId)
	{

	}

	/// @brief 
	/// @param state 
	void Input::State::Merge(const State& state)
	{
		if (_flags < state._flags)
		{
			_flags = state._flags;
			_value = state._value;
			_inputId = state._inputId;
			//_deviceUid = state._deviceUid;
		}
		else if (_flags == state._flags && std::abs(_value) < std::abs(state._value))
		{
			_value = state._value;
			_inputId = state._inputId;
			//_deviceUid = state._deviceUid;
		}
	}

	/// @brief 
	/// @return 
	bool Input::State::IsPressed() const
	{
		return (_flags & Flag::Pressed);
	}

	/// @brief 
	/// @return 
	bool Input::State::IsJustPressed() const
	{
		return (_flags & Flag::JustPressed);
	}

	/// @brief 
	/// @return 
	bool Input::State::IsReleased() const
	{
		return (_flags & Flag::Released);
	}

	/// @brief 
	/// @return 
	bool Input::State::IsJustReleased() const
	{
		return (_flags & Flag::JustReleased);
	}

	/// @brief 
	/// @return 
	bool Input::State::IsJustRepeat() const
	{
		return (_flags & Flag::JustRepeat);
	}

	/// @brief 
	/// @return 
	bool Input::State::IsJustPressedOrRepeat() const
	{
		return (_flags & (Flag::JustPressed | Flag::JustRepeat));
	}
}
