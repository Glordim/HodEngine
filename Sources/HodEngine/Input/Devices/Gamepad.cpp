#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/Gamepad.hpp"
#include "HodEngine/Input/State.hpp"

namespace hod::input
{
	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	Gamepad::Gamepad(const UID& uid, const std::string_view& name, Product product, uint32_t stateSize)
	: Device(Type::PAD, uid, name, product, stateSize)
	, _leftStickX(Identifier(), "Left Stick X", StateView())
	, _leftStickLeft(Identifier(), "Left Stick Left", StateView())
	, _leftStickRight(Identifier(), "Left Stick Right", StateView())
	, _leftStickY(Identifier(), "Left Stick Y", StateView())
	, _leftStickUp(Identifier(), "Left Stick Up", StateView())
	, _leftStickDown(Identifier(), "Left Stick Down", StateView())
	, _leftStickPress(Identifier(), "Left Stick Press", StateView())
	, _rightStickX(Identifier(), "Right Stick X", StateView())
	, _rightStickLeft(Identifier(), "Right Stick Left", StateView())
	, _rightStickRight(Identifier(), "Right Stick Right", StateView())
	, _rightStickY(Identifier(), "Right Stick Y", StateView())
	, _rightStickUp(Identifier(), "Right Stick Up", StateView())
	, _rightStickDown(Identifier(), "Right Stick Down", StateView())
	, _rightStickPress(Identifier(), "Right Stick Press", StateView())
	, _dpadUp(Identifier(), "DPad Up", StateView())
	, _dpadDown(Identifier(), "DPad Down", StateView())
	, _dpadLeft(Identifier(), "DPad Left", StateView())
	, _dpadRight(Identifier(), "DPad Right", StateView())
	, _buttonNorth(Identifier(), "Button North", StateView())
	, _buttonEast(Identifier(), "Button East", StateView())
	, _buttonWest(Identifier(), "Button West", StateView())
	, _buttonSouth(Identifier(), "Button South", StateView())
	, _leftShoulder(Identifier(), "Left Shoulder", StateView())
	, _rightShoulder(Identifier(), "Right Shoulder", StateView())
	, _leftTrigger(Identifier(), "Left Trigger", StateView())
	, _rightTrigger(Identifier(), "Right Trigger", StateView())
	, _start(Identifier(), "Start", StateView())
	, _select(Identifier(), "Select", StateView())
	{
		//AddInput(&_leftStick);
		AddInput(&_leftStickX);
		AddInput(&_leftStickLeft);
		AddInput(&_leftStickRight);
		AddInput(&_leftStickY);
		AddInput(&_leftStickUp);
		AddInput(&_leftStickDown);
		AddInput(&_leftStickPress);

		//AddInput(&_rightStick);
		AddInput(&_rightStickX);
		AddInput(&_rightStickLeft);
		AddInput(&_rightStickRight);
		AddInput(&_rightStickY);
		AddInput(&_rightStickUp);
		AddInput(&_rightStickDown);
		AddInput(&_rightStickPress);

		//AddInput(&_dpad);
		AddInput(&_dpadUp);
		AddInput(&_dpadDown);
		AddInput(&_dpadLeft);
		AddInput(&_dpadRight);

		AddInput(&_buttonNorth);
		AddInput(&_buttonEast);
		AddInput(&_buttonWest);
		AddInput(&_buttonSouth);

		AddInput(&_leftShoulder);
		AddInput(&_rightShoulder);

		AddInput(&_leftTrigger);
		AddInput(&_rightTrigger);

		AddInput(&_start);
		AddInput(&_select);
	}
}
