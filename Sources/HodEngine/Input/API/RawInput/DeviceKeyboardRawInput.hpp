#pragma once
#include "HodEngine/Input/Export.hpp"

#include <Windows.h>

#include "HodEngine/Input/Devices/DeviceKeyboard.hpp"
#include "HodEngine/Input/Input.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DeviceKeyboardRawInput : public DeviceKeyboard
	{
	public:
											DeviceKeyboardRawInput(HANDLE handle, const std::string_view& sName, const RID_DEVICE_INFO_KEYBOARD& info);
											DeviceKeyboardRawInput(const DeviceKeyboardRawInput&) = delete;
											DeviceKeyboardRawInput(DeviceKeyboardRawInput&&) = delete;
											~DeviceKeyboardRawInput() override = default;

		DeviceKeyboardRawInput&				operator = (const DeviceKeyboardRawInput&) = delete;
		DeviceKeyboardRawInput&				operator = (DeviceKeyboardRawInput&&) = delete;

	public:

		HANDLE								GetHandle();

		void								ReadRawInput(const RAWKEYBOARD& rawKeyboard);

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;

	private:

		static UID							ComputeDeviceUID(HANDLE hDevice);

	private:

		HANDLE								_handle = nullptr;
	};
}
