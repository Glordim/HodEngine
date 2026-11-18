#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/GameInput/GameInputGamepad.hpp"

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/FeedbackVibration.hpp"
#include "HodEngine/Input/State.hpp"

#include <HodEngine/Core/Hash.hpp>

#include <Windows.h>
#include <GameInput.h>

#include <cstddef>
#include <cstring>

using namespace GameInput::v4;

namespace hod::inline input
{
	struct PadGameInputState : public State
	{
		GameInputGamepadState _gamepad;
	};

	// The StateViews below read GameInputGamepadState at the start of the state: buttons (uint32_t, bits in little endian bytes),
	// leftTrigger, rightTrigger, leftThumbstickX, leftThumbstickY, rightThumbstickX, rightThumbstickY (float)
	static_assert(offsetof(PadGameInputState, _gamepad) == 0);
	static_assert(offsetof(GameInputGamepadState, buttons) == 0);
	static_assert(offsetof(GameInputGamepadState, leftTrigger) == 4);
	static_assert(offsetof(GameInputGamepadState, rightTrigger) == 8);
	static_assert(offsetof(GameInputGamepadState, leftThumbstickX) == 12);
	static_assert(offsetof(GameInputGamepadState, leftThumbstickY) == 16);
	static_assert(offsetof(GameInputGamepadState, rightThumbstickX) == 20);
	static_assert(offsetof(GameInputGamepadState, rightThumbstickY) == 24);

	/// @brief The StateView of a GameInputGamepadButtons flag (a single bit)
	static StateView ButtonView(GameInputGamepadButtons button)
	{
		const uint32_t mask = static_cast<uint32_t>(button);
		uint32_t       bit = 0;
		while (((mask >> bit) & 1) == 0)
		{
			++bit;
		}
		return StateView(StateView::Format::Bit, offsetof(GameInputGamepadState, buttons) + bit / 8, bit % 8);
	}

	/// @brief
	/// @param device
	GameInputGamepad::GameInputGamepad(IGameInputDevice* device)
	: Gamepad(ComputeDeviceUID(device), ComputeName(device), ComputeProduct(device), sizeof(PadGameInputState))
	, _device(device)
	{
		_device->AddRef();

		const StateView leftStickX(StateView::Format::F32, offsetof(GameInputGamepadState, leftThumbstickX));
		const StateView leftStickY(StateView::Format::F32, offsetof(GameInputGamepadState, leftThumbstickY));
		const StateView rightStickX(StateView::Format::F32, offsetof(GameInputGamepadState, rightThumbstickX));
		const StateView rightStickY(StateView::Format::F32, offsetof(GameInputGamepadState, rightThumbstickY));

		_leftStickX.SetStateView(leftStickX);
		_leftStickLeft.SetStateView(leftStickX);
		_leftStickRight.SetStateView(leftStickX);
		_leftStickY.SetStateView(leftStickY);
		_leftStickUp.SetStateView(leftStickY);
		_leftStickDown.SetStateView(leftStickY);
		_leftStickPress.SetStateView(ButtonView(GameInputGamepadLeftThumbstick));

		_rightStickX.SetStateView(rightStickX);
		_rightStickLeft.SetStateView(rightStickX);
		_rightStickRight.SetStateView(rightStickX);
		_rightStickY.SetStateView(rightStickY);
		_rightStickUp.SetStateView(rightStickY);
		_rightStickDown.SetStateView(rightStickY);
		_rightStickPress.SetStateView(ButtonView(GameInputGamepadRightThumbstick));

		_dpadUp.SetStateView(ButtonView(GameInputGamepadDPadUp));
		_dpadDown.SetStateView(ButtonView(GameInputGamepadDPadDown));
		_dpadLeft.SetStateView(ButtonView(GameInputGamepadDPadLeft));
		_dpadRight.SetStateView(ButtonView(GameInputGamepadDPadRight));

		// Positions, as on an Xbox gamepad (GameInput maps the others' buttons to these)
		_buttonNorth.SetStateView(ButtonView(GameInputGamepadY));
		_buttonEast.SetStateView(ButtonView(GameInputGamepadB));
		_buttonWest.SetStateView(ButtonView(GameInputGamepadX));
		_buttonSouth.SetStateView(ButtonView(GameInputGamepadA));

		_leftShoulder.SetStateView(ButtonView(GameInputGamepadLeftShoulder));
		_rightShoulder.SetStateView(ButtonView(GameInputGamepadRightShoulder));

		_leftTrigger.SetStateView(StateView(StateView::Format::F32, offsetof(GameInputGamepadState, leftTrigger)));
		_rightTrigger.SetStateView(StateView(StateView::Format::F32, offsetof(GameInputGamepadState, rightTrigger)));

		_start.SetStateView(ButtonView(GameInputGamepadMenu));
		_select.SetStateView(ButtonView(GameInputGamepadView));
	}

	/// @brief
	GameInputGamepad::~GameInputGamepad()
	{
		_device->Release();
	}

	/// @brief
	/// @param gamepadState
	void GameInputGamepad::WriteState(const GameInputGamepadState& gamepadState)
	{
		std::memcpy(&EditNextState<PadGameInputState>()->_gamepad, &gamepadState, sizeof(gamepadState));
	}

	/// @brief Nothing pressed (a disconnected gamepad)
	void GameInputGamepad::ClearState()
	{
		std::memset(&EditNextState<PadGameInputState>()->_gamepad, 0, sizeof(GameInputGamepadState));
	}

	/// @brief
	/// @param feedback
	/// @return
	bool GameInputGamepad::ApplyFeedback(Feedback& feedback)
	{
		if (feedback.GetRttiType() == FeedbackVibration::GetRttiTypeStatic())
		{
			FeedbackVibration& feedbackVibration = static_cast<FeedbackVibration&>(feedback);
			Vibrate(feedbackVibration._left, feedbackVibration._right);
			return true;
		}
		return false;
	}

	/// @brief
	/// @param left The low frequency motor, in [0, 1]
	/// @param right The high frequency motor, in [0, 1]
	void GameInputGamepad::Vibrate(float left, float right)
	{
		GameInputRumbleParams params = {};
		params.lowFrequency = left;
		params.highFrequency = right;
		_device->SetRumbleState(&params);
	}

	/// @brief
	/// @return
	IGameInputDevice* GameInputGamepad::GetGameInputDevice() const
	{
		return _device;
	}

	/// @brief
	/// @param device The same device (IsSameDevice)
	void GameInputGamepad::SetGameInputDevice(IGameInputDevice* device)
	{
		if (device != _device)
		{
			device->AddRef();
			_device->Release();
			_device = device;
		}
	}

	/// @brief
	/// @param device
	/// @return
	bool GameInputGamepad::IsSameDevice(IGameInputDevice* device) const
	{
		if (device == _device)
		{
			return true;
		}

		APP_LOCAL_DEVICE_ID id;
		APP_LOCAL_DEVICE_ID otherId;
		_device->GetDeviceId(&id);
		device->GetDeviceId(&otherId);
		return std::memcmp(id.value, otherId.value, sizeof(id.value)) == 0;
	}

	/// @brief The device id of GameInput (stable for this device on this machine), hashed
	/// @param device
	/// @return
	UID GameInputGamepad::ComputeDeviceUID(IGameInputDevice* device)
	{
		APP_LOCAL_DEVICE_ID id;
		device->GetDeviceId(&id);
		return UID(Api::DeviceUidOffset::GAMEINPUT, Hash::ComputeXxh3_64(id.value, sizeof(id.value)));
	}

	/// @brief
	/// @param device
	/// @return
	Device::Product GameInputGamepad::ComputeProduct(IGameInputDevice* device)
	{
		constexpr uint16_t microsoftVendorId = 0x045E;
		constexpr uint16_t sonyVendorId = 0x054C;

		const GameInputDeviceFamily family = device->GetDeviceFamily();
		if (family == GameInputFamilyXboxOne || family == GameInputFamilyXbox360 || device->GetVendorId() == microsoftVendorId)
		{
			return Product::XBOX;
		}

		if (device->GetVendorId() == sonyVendorId)
		{
			switch (device->GetProductId())
			{
				case 0x0268: return Product::DUALSHOCK_3;
				case 0x05C4:
				case 0x09CC: return Product::DUALSHOCK_4;
				case 0x0CE6:
				case 0x0DF2: return Product::DUALSENSE; // DualSense, DualSense Edge
				default: break;
			}
		}

		return Product::UNKNOWN;
	}

	/// @brief
	/// @param device
	/// @return Its display name (UTF-8), "Gamepad" without one
	String GameInputGamepad::ComputeName(IGameInputDevice* device)
	{
		char           buffer[256] = {};
		const uint32_t size = device->GetDisplayName(sizeof(buffer), buffer); // Its size, terminator included
		if (size <= 1 || size > sizeof(buffer))
		{
			return "Gamepad";
		}
		return String(buffer);
	}
}
