#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/DeviceMouse.hpp"
#include "HodEngine/Input/Input.hpp"

#include <Windows.h>

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DeviceMouseRawInput : public DeviceMouse
	{
	public:

										DeviceMouseRawInput(HANDLE handle, const std::string_view& name, const RID_DEVICE_INFO_MOUSE& info);
										DeviceMouseRawInput(const DeviceMouseRawInput&) = delete;
										DeviceMouseRawInput(DeviceMouseRawInput&&) = delete;
										~DeviceMouseRawInput() override = default;

		DeviceMouseRawInput&			operator = (const DeviceMouseRawInput&) = delete;
		DeviceMouseRawInput&			operator = (DeviceMouseRawInput&&) = delete;

	public:

		HANDLE							GetHandle();

		void							ResyncLastCusorPosition();
		void							SimulateMouseDownBeforeFocusGain(InputId mouseButtonInputId);
		void							ReadRawInput(const RAWMOUSE& rawMouse);

	protected:

		bool							ApplyFeedback(Feedback& feedback) override;

		void							ResetNextState() override;

	private:

		static UID						ComputeDeviceUID(HANDLE hDevice);

	private:

		HANDLE							_handle = nullptr;

		int32_t							_lastAbsoluteX = 0;
		int32_t							_lastAbsoluteY = 0;
		bool							_lastAbsoluteDirty = true;
	};
}
