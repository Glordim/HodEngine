#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Device.hpp"
#include "HodEngine/Input/State.hpp"

#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/InputAxis.hpp"
//#include "HodEngine/Input/InputVector2.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API Gamepad : public Device
	{
	public:

						Gamepad(const UID& uid, const std::string_view& name, Product product, uint32_t stateSize);
						Gamepad(const Gamepad&) = delete;
						Gamepad(Gamepad&&) = delete;
						~Gamepad() override = default;

		Gamepad&		operator = (const Gamepad&) = delete;
		Gamepad&		operator = (Gamepad&&) = delete;

	public:

		static inline const Gamepad* Current() { return _current; }

	protected:

		inline void MakeCurrent() override { _current = this; }

	protected:

		static Gamepad* _current;

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
