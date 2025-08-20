#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/DeviceMouse.hpp"
#include "HodEngine/Input/State.hpp"

namespace hod::input
{
	struct MouseState : public State
	{
		int16_t		_delta[2];
		int8_t		_wheel;
		uint8_t		_buttons;
	};

	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	DeviceMouse::DeviceMouse(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::MOUSE, uid, name, product, sizeof(MouseState))
	//, _axis(Identifier(), "Axis", StateView(0, 0, 1))
	, _axisX(Identifier(), "AxisX", StateView(StateView::Format::S16, 0, 0))
	, _axisY(Identifier(), "AxisY", StateView(StateView::Format::S16, 0, 0))
	, _axisWheel(Identifier(), "Axis Wheel", StateView(StateView::Format::Bit, 0, 0))
	, _buttonLeft(Identifier(), "Button Left", StateView(StateView::Format::Bit, 0, 0))
	, _buttonRight(Identifier(), "Button Right", StateView(StateView::Format::Bit, 0, 0))
	, _buttonWheel(Identifier(), "Button Wheel", StateView(StateView::Format::Bit, 0, 0))
	, _button4(Identifier(), "Button 4", StateView(StateView::Format::Bit, 0, 0))
	, _button5(Identifier(), "Button 5", StateView(StateView::Format::Bit, 0, 0))
	{
		AddInput(&_axisX);
		AddInput(&_axisY);

		AddInput(&_axisWheel);

		AddInput(&_buttonLeft);
		AddInput(&_buttonRight);
		AddInput(&_buttonWheel);
		AddInput(&_button4);
		AddInput(&_button5);
	}
}
