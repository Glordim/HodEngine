#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/Mouse.hpp"
#include "HodEngine/Input/State.hpp"

namespace hod::input
{
	struct MouseState : public State
	{
		int16_t _delta[2];
		int8_t  _wheel;
		uint8_t _buttons;
	};

	Mouse* Mouse::_current = nullptr;

	/// @brief
	/// @param uid
	/// @param name
	/// @param product
	Mouse::Mouse(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::MOUSE, uid, name, product, sizeof(MouseState))
	//, _axis(Identifier(), "Axis", StateView(0, 0, 1))
	, _axisX(Identifier(), "AxisX", StateView(StateView::Format::S16, 0, 0))
	, _axisY(Identifier(), "AxisY", StateView(StateView::Format::S16, 2, 0))
	, _axisWheel(Identifier(), "Axis Wheel", StateView(StateView::Format::S8, 4, 0))
	, _buttonLeft(Identifier(), "Button Left", StateView(StateView::Format::Bit, 5, 0))
	, _buttonRight(Identifier(), "Button Right", StateView(StateView::Format::Bit, 5, 1))
	, _buttonWheel(Identifier(), "Button Wheel", StateView(StateView::Format::Bit, 5, 2))
	, _buttonForward(Identifier(), "Button Forward", StateView(StateView::Format::Bit, 5, 3))
	, _buttonBackward(Identifier(), "Button Backward", StateView(StateView::Format::Bit, 5, 4))
	{
		AddInput(&_axisX);
		AddInput(&_axisY);

		AddInput(&_axisWheel);

		AddInput(&_buttonLeft);
		AddInput(&_buttonRight);
		AddInput(&_buttonWheel);

		AddInput(&_buttonForward);
		AddInput(&_buttonBackward);
	}
}
