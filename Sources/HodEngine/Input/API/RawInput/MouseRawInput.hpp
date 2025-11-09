#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Mouse.hpp"
#include "HodEngine/Input/Input.hpp"

using HANDLE = void*;
struct tagRID_DEVICE_INFO_MOUSE;
struct tagRAWMOUSE;

namespace hod::input
{
	/// @brief
	class HOD_INPUT_API MouseRawInput : public Mouse
	{
	public:
		MouseRawInput(HANDLE handle, const std::string_view& name, const tagRID_DEVICE_INFO_MOUSE& info);
		MouseRawInput(const MouseRawInput&) = delete;
		MouseRawInput(MouseRawInput&&) = delete;
		~MouseRawInput() override = default;

		MouseRawInput& operator=(const MouseRawInput&) = delete;
		MouseRawInput& operator=(MouseRawInput&&) = delete;

	public:
		HANDLE GetHandle();

		void ResyncLastCusorPosition();
		void SimulateMouseDownBeforeFocusGain(InputId mouseButtonInputId);
		void ReadRawInput(const tagRAWMOUSE& rawMouse);

	protected:
		bool ApplyFeedback(Feedback& feedback) override;

		void ResetNextState() override;

	private:
		static UID ComputeDeviceUID(HANDLE hDevice);

	private:
		HANDLE _handle = nullptr;

		int32_t _lastAbsoluteX = 0;
		int32_t _lastAbsoluteY = 0;
		bool    _lastAbsoluteDirty = true;
	};
}
