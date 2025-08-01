#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/DeviceKeyboard.hpp"
#include "HodEngine/Input/Input.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DeviceKeyboardWindowLib : public DeviceKeyboard
	{
	public:
											DeviceKeyboardWindowLib();
											DeviceKeyboardWindowLib(const DeviceKeyboardWindowLib&) = delete;
											DeviceKeyboardWindowLib(DeviceKeyboardWindowLib&&) = delete;
											~DeviceKeyboardWindowLib() override = default;

		DeviceKeyboardWindowLib&			operator = (const DeviceKeyboardWindowLib&) = delete;
		DeviceKeyboardWindowLib&			operator = (DeviceKeyboardWindowLib&&) = delete;

		void								OnKeyPressed(int keycode);
		void								OnKeyReleased(int keycode);

	protected:

		bool								ApplyFeedback(Feedback& feedback) override;

		bool								_nextState[256];
	};
}
