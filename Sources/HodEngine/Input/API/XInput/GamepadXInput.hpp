#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Gamepad.hpp"
#include "HodEngine/Input/Input.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"
#include "HodEngine/Core/UID.hpp"

struct _XINPUT_STATE;
using DWORD = unsigned long;

namespace hod::input
{
	/// @brief
	class HOD_INPUT_API GamepadXInput : public Gamepad
	{
	private:
		static constexpr short DeadZone = 3500;

	public:
		GamepadXInput(uint32_t padIndex);
		GamepadXInput(const GamepadXInput&) = delete;
		GamepadXInput(GamepadXInput&&) = delete;
		~GamepadXInput() override = default;

		GamepadXInput& operator=(const GamepadXInput&) = delete;
		GamepadXInput& operator=(GamepadXInput&&) = delete;

		bool ApplyFeedback(Feedback& feedback) override;

		void Vibrate(float left, float right);

		void WriteState(const _XINPUT_STATE& xInputState);

		uint32_t GetPadIndex() const;

	private:
		static UID ComputeDeviceUID(uint32_t padIndex);

		static float GetAxisValue(int32_t rawValue, int32_t deadZone = 0);
		static float GetButtonValue(uint32_t buttons, uint32_t mask);
		static float GetTriggerValue(uint32_t rawValue);

	private:
		DWORD    _lastPacketNumber = 0;
		uint32_t _padIndex = 0;

		SystemTime::TimeStamp _lastUpdate;
	};
}
