#include "HodEngine/Input/Pch.hpp"

#include <Windows.h>
#include <Xinput.h>

#include "HodEngine/Input/API/XInput/GamepadXInput.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"

#include "HodEngine/Input/FeedbackVibration.hpp"

#include <cstring>
#include <cmath>

namespace hod::input
{
	struct PadXboxState : public State
	{
		XINPUT_GAMEPAD _xInputGamepad;
	};

	/// @brief 
	/// @param api 
	/// @param padIndex 
	GamepadXInput::GamepadXInput(uint32_t padIndex)
		: Gamepad(ComputeDeviceUID(padIndex), "Xbox_Pad", Product::XBOX, sizeof(PadXboxState))
		, _padIndex(padIndex)
	{
		_lastUpdate = SystemTime::Now();

		_leftStickX.SetStateView(StateView(StateView::Format::S16, 4));
		_leftStickLeft.SetStateView(StateView(StateView::Format::S16, 4));
		_leftStickRight.SetStateView(StateView(StateView::Format::S16, 4));
		_leftStickY.SetStateView(StateView(StateView::Format::S16, 6));
		_leftStickUp.SetStateView(StateView(StateView::Format::S16, 6));
		_leftStickDown.SetStateView(StateView(StateView::Format::S16, 6));
		_leftStickPress.SetStateView(StateView(StateView::Format::Bit, 0, 6));

		_rightStickX.SetStateView(StateView(StateView::Format::S16, 8));
		_rightStickLeft.SetStateView(StateView(StateView::Format::S16, 8));
		_rightStickRight.SetStateView(StateView(StateView::Format::S16, 8));
		_rightStickY.SetStateView(StateView(StateView::Format::S16, 10));
		_rightStickUp.SetStateView(StateView(StateView::Format::S16, 10));
		_rightStickDown.SetStateView(StateView(StateView::Format::S16, 10));
		_rightStickPress.SetStateView(StateView(StateView::Format::Bit, 0, 7));

		_dpadUp.SetStateView(StateView(StateView::Format::Bit, 0, 0));
		_dpadDown.SetStateView(StateView(StateView::Format::Bit, 0, 1));
		_dpadLeft.SetStateView(StateView(StateView::Format::Bit, 0, 2));
		_dpadRight.SetStateView(StateView(StateView::Format::Bit, 0, 3));

		_buttonNorth.SetStateView(StateView(StateView::Format::Bit, 1, 7));
		_buttonEast.SetStateView(StateView(StateView::Format::Bit, 1, 5));
		_buttonWest.SetStateView(StateView(StateView::Format::Bit, 1, 6));
		_buttonSouth.SetStateView(StateView(StateView::Format::Bit, 1, 4));

		_leftShoulder.SetStateView(StateView(StateView::Format::Bit, 1, 0));
		_rightShoulder.SetStateView(StateView(StateView::Format::Bit, 1, 1));

		_leftTrigger.SetStateView(StateView(StateView::Format::U8, 2));
		_rightTrigger.SetStateView(StateView(StateView::Format::U8, 3));

		_start.SetStateView(StateView(StateView::Format::Bit, 0, 4));
		_select.SetStateView(StateView(StateView::Format::Bit, 0, 5));
	}

	void GamepadXInput::WriteState(const XINPUT_STATE& xInputState)
	{
		if (_lastPacketNumber < xInputState.dwPacketNumber)
		{
			_lastPacketNumber = xInputState.dwPacketNumber;
			std::memcpy(&EditNextState<PadXboxState>()->_xInputGamepad, &xInputState.Gamepad, sizeof(xInputState.Gamepad));
		}
	}
	/*
		bool bConnected = false;

		SystemTime::TimeStamp now = SystemTime::Now();
		if (IsConnected() == false && SystemTime::ElapsedTimeInSeconds(_lastUpdate, now) < (1.0f + (_padIndex * 0.15f)))
		{
			return false; // Skip update for disconnected device because fetching connection has a very high cost
		}

		_lastUpdate = now;

		XINPUT_STATE state;
		std::memset(&state, 0, sizeof(XINPUT_STATE));
		if (_api->GetPadState(_padIndex, &state) == true)
		{*/
			/*
			// Sticks
			SetInputValue(_axisLeftX, GetAxisValue(state.Gamepad.sThumbLX, DeadZone));
			SetInputValue(_axisLeftY, -GetAxisValue(state.Gamepad.sThumbLY, DeadZone));
			SetInputValue(_axisRightX, GetAxisValue(state.Gamepad.sThumbRX, DeadZone));
			SetInputValue(_axisRightY, -GetAxisValue(state.Gamepad.sThumbRY, DeadZone));

			// Buttons
			SetInputValue(_buttonA, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_A));
			SetInputValue(_buttonB, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_B));
			SetInputValue(_buttonY, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_Y));
			SetInputValue(_buttonX, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_X));
			SetInputValue(_buttonDpadUp, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP));
			SetInputValue(_buttonDpadDown, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN));
			SetInputValue(_buttonDpadLeft, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT));
			SetInputValue(_buttonDpadRight, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT));
			SetInputValue(_buttonLB, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER));
			SetInputValue(_buttonLT, GetTriggerValue(state.Gamepad.bLeftTrigger));
			SetInputValue(_buttonLS, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB));
			SetInputValue(_buttonRB, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER));
			SetInputValue(_buttonRT, GetTriggerValue(state.Gamepad.bRightTrigger));
			SetInputValue(_buttonRS, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB));
			SetInputValue(_buttonStart, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_START));
			SetInputValue(_buttonBack, GetButtonValue(state.Gamepad.wButtons, XINPUT_GAMEPAD_BACK));
			*/
/*/
			bConnected = true;
		}

		SetConnected(bConnected);
		return bConnected;
	}
		*/

	/// @brief 
	/// @param feedback 
	/// @return 
	bool GamepadXInput::ApplyFeedback(Feedback& feedback)
	{
		switch (feedback.GetMetaType())
		{
			case FeedbackVibration::GetMetaTypeStatic():
			{
				FeedbackVibration& feedbackVibration = static_cast<FeedbackVibration&>(feedback);
				Vibrate(feedbackVibration._left, feedbackVibration._right);
				return true;
			}
			break;

			default:
			{
				return false;
			}
			break;
		}
	}

	/// @brief 
	/// @param left 
	/// @param right 
	void GamepadXInput::Vibrate(float left, float right)
	{
		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = (WORD)(left * 65535.0f);
		vibration.wRightMotorSpeed = (WORD)(right * 65535.0f);

		//_api->SetPadState(_padIndex, &vibration);
	}

	/// @brief 
	/// @param rawValue 
	/// @param deadZone 
	/// @return 
	float GamepadXInput::GetAxisValue(int32_t rawValue, int32_t deadZone)
	{
		float result = 0.0f;
		if (std::abs(rawValue) > deadZone)
		{
			if (rawValue > 0)
			{
				rawValue++;	// positive values need to be mapped on [1, 32768], instead of [0, 32767]
								// note : negative values are mapped on [-32768, -1]
			}

			rawValue += (rawValue > 0.0f) ? -deadZone : deadZone;
			result = (float)rawValue / (float)(32768 - deadZone);
		}

		return result;
	}

	/// @brief 
	/// @param padIndex 
	/// @return 
	UID GamepadXInput::ComputeDeviceUID(uint32_t padIndex)
	{
		return UID(Api::DeviceUidOffset::XINPUT, padIndex);
	}

	/// @brief 
	/// @param buttons 
	/// @param mask 
	/// @return 
	float GamepadXInput::GetButtonValue(uint32_t buttons, uint32_t mask)
	{
		return ((buttons & mask) == 0) ? 0.0f : 1.0f;
	}

	/// @brief 
	/// @param rawValue 
	/// @return 
	float GamepadXInput::GetTriggerValue(uint32_t rawValue)
	{
		return (float)rawValue / 255.0f;
	}

	uint32_t GamepadXInput::GetPadIndex() const
	{
		return _padIndex;
	}
}
