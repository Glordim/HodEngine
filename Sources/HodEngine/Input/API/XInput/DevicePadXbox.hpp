#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/DevicePad.hpp"
#include "HodEngine/Input/Input.hpp"

#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"

namespace hod::input
{
	class ApiXInput;

	/// @brief 
	class HOD_INPUT_API DevicePadXbox : public DevicePad
	{
	private:

		static constexpr short		DeadZone = 3500;

	public:

									DevicePadXbox(ApiXInput* api, uint32_t padIndex);
									DevicePadXbox(const DevicePadXbox&) = delete;
									DevicePadXbox(DevicePadXbox&&) = delete;
									~DevicePadXbox() override = default;

		DevicePadXbox&				operator = (const DevicePadXbox&) = delete;
		DevicePadXbox&				operator = (DevicePadXbox&&) = delete;

		bool						Update();
		bool						ApplyFeedback(Feedback& feedback) override;

		void						Vibrate(float left, float right);

	private:

		static UID					ComputeDeviceUID(uint32_t padIndex);

		static float				GetAxisValue(int32_t rawValue, int32_t deadZone = 0);
		static float				GetButtonValue(uint32_t buttons, uint32_t mask);
		static float				GetTriggerValue(uint32_t rawValue);

	private:

		ApiXInput*					_api = nullptr;
		uint32_t					_padIndex = 0;

		SystemTime::TimeStamp		_lastUpdate;

		// Sticks
		Input						_axisLeftX = (InputId::PadAxisLeftX);
		Input						_axisLeftY = (InputId::PadAxisLeftY);
		Input						_axisRightX = (InputId::PadAxisRightX);
		Input						_axisRightY = (InputId::PadAxisRightY);

		// Buttons
		Input						_buttonA = (InputId::PadButtonA);
		Input						_buttonB = (InputId::PadButtonB);
		Input						_buttonY = (InputId::PadButtonY);
		Input						_buttonX = (InputId::PadButtonX);

		Input						_buttonDpadUp = (InputId::PadButtonDpadUp);
		Input						_buttonDpadDown = (InputId::PadButtonDpadDown);
		Input						_buttonDpadLeft = (InputId::PadButtonDpadLeft);
		Input						_buttonDpadRight = (InputId::PadButtonDpadRight);

		Input						_buttonLB = (InputId::PadButtonLb);
		Input						_buttonLT = (InputId::PadButtonLt);
		Input						_buttonLS = (InputId::PadButtonLs);

		Input						_buttonRB = (InputId::PadButtonRb);
		Input						_buttonRT = (InputId::PadButtonRt);
		Input						_buttonRS = (InputId::PadButtonRs);

		Input						_buttonStart = (InputId::PadButtonStart);
		Input						_buttonBack = (InputId::PadButtonBack);
	};
}
