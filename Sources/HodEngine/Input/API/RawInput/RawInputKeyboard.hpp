#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Keyboard.hpp"
#include "HodEngine/Input/Input.hpp"

struct tagRID_DEVICE_INFO_KEYBOARD;
struct tagRAWKEYBOARD;
using HANDLE = void*;

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API RawInputKeyboard : public Keyboard
	{
	public:
		RawInputKeyboard(HANDLE handle, const std::string_view& sName, const tagRID_DEVICE_INFO_KEYBOARD& info);
		RawInputKeyboard(const RawInputKeyboard&) = delete;
		RawInputKeyboard(RawInputKeyboard&&) = delete;
		~RawInputKeyboard() override = default;

		RawInputKeyboard& operator=(const RawInputKeyboard&) = delete;
		RawInputKeyboard& operator=(RawInputKeyboard&&) = delete;

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
