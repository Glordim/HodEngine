#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Keyboard.hpp"
#include "HodEngine/Input/Input.hpp"

struct tagRID_DEVICE_INFO_KEYBOARD;
struct tagRAWKEYBOARD;
using HANDLE = void*;

namespace hod::input
{
	/// @brief
	class HOD_INPUT_API KeyboardRawInput : public Keyboard
	{
	public:
		KeyboardRawInput(HANDLE handle, const std::string_view& sName, const tagRID_DEVICE_INFO_KEYBOARD& info);
		KeyboardRawInput(const KeyboardRawInput&) = delete;
		KeyboardRawInput(KeyboardRawInput&&) = delete;
		~KeyboardRawInput() override = default;

		KeyboardRawInput& operator=(const KeyboardRawInput&) = delete;
		KeyboardRawInput& operator=(KeyboardRawInput&&) = delete;

	public:
		HANDLE GetHandle() const;

		void ReadRawInput(const tagRAWKEYBOARD& rawKeyboard);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;

	private:
		static UID ComputeDeviceUID(HANDLE hDevice);

	private:
		HANDLE _handle = nullptr;
	};
}
