#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Device.hpp"
#include "HodEngine/Input/State.hpp"

#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/InputAxis.hpp"
//#include "HodEngine/Input/InputVector2.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DevicePad : public Device
	{
	public:

						DevicePad(const UID& uid, const std::string_view& name, Product product, uint32_t stateSize);
						DevicePad(const DevicePad&) = delete;
						DevicePad(DevicePad&&) = delete;
						~DevicePad() override = default;

		DevicePad&		operator = (const DevicePad&) = delete;
		DevicePad&		operator = (DevicePad&&) = delete;

	public:

		//InputVector2	_leftStick;
		InputAxis		_leftStickX;
		InputAxis		_leftStickLeft;
		InputAxis		_leftStickRight;
		InputAxis		_leftStickY;
		InputAxis		_leftStickUp;
		InputAxis		_leftStickDown;
		InputButton		_leftStickPress;

		//InputVector2	_rightStick;
		InputAxis		_rightStickX;
		InputAxis		_rightStickLeft;
		InputAxis		_rightStickRight;
		InputAxis		_rightStickY;
		InputAxis		_rightStickUp;
		InputAxis		_rightStickDown;
		InputButton		_rightStickPress;

		//InputVector2	_dpad;
		InputButton		_dpadUp;
		InputButton		_dpadDown;
		InputButton		_dpadLeft;
		InputButton		_dpadRight;

		InputButton		_buttonNorth;
		InputButton		_buttonEast;
		InputButton		_buttonWest;
		InputButton		_buttonSouth;

		InputButton		_leftShoulder;
		InputButton		_rightShoulder;
		
		InputAxis		_leftTrigger;
		InputAxis		_rightTrigger;
		
		InputButton		_start;
		InputButton		_select;
	};
}
