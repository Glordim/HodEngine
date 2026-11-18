#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Gamepad.hpp"

#include <HodEngine/Core/UID.hpp>

#include <cstdint>

namespace GameInput::v4
{
	struct IGameInputDevice;
	struct GameInputGamepadState;
}

namespace hod::inline input
{
	/// @brief A gamepad seen by GameInput (Xbox, DualShock 4, DualSense, Switch Pro...), its state the GameInputGamepadState as given
	class HOD_INPUT_API GameInputGamepad : public Gamepad
	{
	public:
		GameInputGamepad(::GameInput::v4::IGameInputDevice* device);
		GameInputGamepad(const GameInputGamepad&) = delete;
		GameInputGamepad(GameInputGamepad&&) = delete;
		~GameInputGamepad() override;

		GameInputGamepad& operator=(const GameInputGamepad&) = delete;
		GameInputGamepad& operator=(GameInputGamepad&&) = delete;

		bool ApplyFeedback(Feedback& feedback) override;

		void Vibrate(float left, float right, float leftTrigger = 0.0f, float rightTrigger = 0.0f);

		void WriteState(const ::GameInput::v4::GameInputGamepadState& gamepadState);
		void ClearState();

		::GameInput::v4::IGameInputDevice* GetGameInputDevice() const;
		void                               SetGameInputDevice(::GameInput::v4::IGameInputDevice* device);

		/// @brief The same physical device (its device id), whatever the IGameInputDevice
		bool IsSameDevice(::GameInput::v4::IGameInputDevice* device) const;

	private:
		static UID     ComputeDeviceUID(::GameInput::v4::IGameInputDevice* device);
		static Product ComputeProduct(::GameInput::v4::IGameInputDevice* device);
		static String  ComputeName(::GameInput::v4::IGameInputDevice* device);

	private:
		::GameInput::v4::IGameInputDevice* _device = nullptr; // Referenced
	};
}
