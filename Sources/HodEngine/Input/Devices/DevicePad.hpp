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

						DevicePad(const UID& uid, const std::string_view& name, Product product);
						DevicePad(const DevicePad&) = delete;
						DevicePad(DevicePad&&) = delete;
						~DevicePad() override = default;

		DevicePad&		operator = (const DevicePad&) = delete;
		DevicePad&		operator = (DevicePad&&) = delete;

	public:

	/*
		InputButton		_buttonNorth;
		InputButton		_buttonEast;
		InputButton		_buttonWest;
		InputButton		_buttonSouth;

		InputButton		_leftShoulder;
		InputButton		_rightShoulder;

		InputButton		_leftStickPress;
		InputButton		_rightStickPress;

		InputButton		_start;
		InputButton		_select;

		InputButton		_dpadUp;
		InputButton		_dpadDown;
		InputButton		_dpadLeft;
		InputButton		_dpadRight;

		InputAxis		_leftTrigger;
		InputAxis		_rightTrigger;
	*/

		//InputVector2	_leftStick;
		//InputVector2	_rightStick;

		//InputVector2	_dpad;
	};
}
