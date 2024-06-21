#pragma once

#include "HodEngine/Input/Devices/DeviceMouse.hpp"
#include "HodEngine/Input/Input.hpp"

#include <Windows.h>

namespace hod::input
{
	/// @brief 
	class DeviceMouseRawInput : public DeviceMouse
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

		void							PrepareUpdate();

	protected:

		bool							ApplyFeedback(Feedback& feedback) override;

	private:

		static UID						ComputeDeviceUID(HANDLE hDevice);

	private:

		HANDLE							_handle = nullptr;

		int32_t							_lastAbsoluteX = 0;
		int32_t							_lastAbsoluteY = 0;
		bool							_lastAbsoluteDirty = true;

		// Move
		Input							_axisX = (InputId::MouseAxisX);
		Input							_axisY = (InputId::MouseAxisY);

		// Whell
		Input							_wheel = (InputId::MouseAxisWheel);

		// Buttons
		Input							_button0; // Setup in constructor (can be swapped, left-handed mode)
		Input							_button1; // Setup in constructor (can be swapped, left-handed mode)
		Input							_button2 = (InputId::MouseButtonWheel);
		Input							_button3 = (InputId::MouseButton3);
		Input							_button4 = (InputId::MouseButton4);		
	};
}
