#pragma once
#include "HodEngine/Input/Export.hpp"

#include <Windows.h>

#include "HodEngine/Input/Devices/Keyboard.hpp"
#include "HodEngine/Input/Input.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API KeyboardRawInput : public Keyboard
	{
	public:
											KeyboardRawInput(HANDLE handle, const std::string_view& sName, const RID_DEVICE_INFO_KEYBOARD& info);
											KeyboardRawInput(const KeyboardRawInput&) = delete;
											KeyboardRawInput(KeyboardRawInput&&) = delete;
											~KeyboardRawInput() override = default;

		KeyboardRawInput&					operator = (const KeyboardRawInput&) = delete;
		KeyboardRawInput&					operator = (KeyboardRawInput&&) = delete;

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
