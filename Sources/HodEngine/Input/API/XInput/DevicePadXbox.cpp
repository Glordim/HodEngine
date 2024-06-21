#include <Windows.h>
#include <Xinput.h>

#include "HodEngine/Input/API/XInput/DevicePadXbox.hpp"
#include "HodEngine/Input/API/XInput/ApiXInput.hpp"

#include "HodEngine/Input/FeedbackVibration.hpp"

#include <cstring>
#include <cmath>

namespace hod::input
{
	/// @brief 
	/// @param api 
	/// @param padIndex 
	DevicePadXbox::DevicePadXbox(ApiXInput* api, uint32_t padIndex)
		: DevicePad(ComputeDeviceUID(padIndex), "Xbox_Pad", Product::XBOX)
		, _api(api)
		, _padIndex(padIndex)
	{
		// Sticks
		AddInput(&_axisLeftX);
		AddInput(&_axisLeftY);
		AddInput(&_axisRightX);
		AddInput(&_axisRightY);

		// Buttons
		AddInput(&_buttonA);
		AddInput(&_buttonB);
		AddInput(&_buttonY);
		AddInput(&_buttonX);

		AddInput(&_buttonDpadUp);
		AddInput(&_buttonDpadDown);
		AddInput(&_buttonDpadLeft);
		AddInput(&_buttonDpadRight);

		AddInput(&_buttonLB);
		AddInput(&_buttonLT);
		AddInput(&_buttonLS);

		AddInput(&_buttonRB);
		AddInput(&_buttonRT);
		AddInput(&_buttonRS);

		AddInput(&_buttonStart);
		AddInput(&_buttonBack);

		_lastUpdate = SystemTime::Now();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool DevicePadXbox::Update()
	{
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
		{
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

			bConnected = true;
		}

		SetConnected(bConnected);
		return bConnected;
	}

	/// @brief 
	/// @param feedback 
	/// @return 
	bool DevicePadXbox::ApplyFeedback(Feedback& feedback)
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
	void DevicePadXbox::Vibrate(float left, float right)
	{
		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = (WORD)(left * 65535.0f);
		vibration.wRightMotorSpeed = (WORD)(right * 65535.0f);

		_api->SetPadState(_padIndex, &vibration);
	}

	/// @brief 
	/// @param rawValue 
	/// @param deadZone 
	/// @return 
	float DevicePadXbox::GetAxisValue(int32_t rawValue, int32_t deadZone)
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
	UID DevicePadXbox::ComputeDeviceUID(uint32_t padIndex)
	{
		return UID(Api::DeviceUidOffset::XINPUT, padIndex);
	}

	/// @brief 
	/// @param buttons 
	/// @param mask 
	/// @return 
	float DevicePadXbox::GetButtonValue(uint32_t buttons, uint32_t mask)
	{
		return ((buttons & mask) == 0) ? 0.0f : 1.0f;
	}

	/// @brief 
	/// @param rawValue 
	/// @return 
	float DevicePadXbox::GetTriggerValue(uint32_t rawValue)
	{
		return (float)rawValue / 255.0f;
	}
}
