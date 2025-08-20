#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/DevicePad.hpp"
#include "HodEngine/Input/State.hpp"

namespace hod::input
{
	struct PadState : public State
	{

	};

	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	DevicePad::DevicePad(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::PAD, uid, name, product, sizeof(PadState))
	{
		/*
		// Sticks
		AddInput(&_axisLeftX);
		AddInput(&_axisLeftY);
		AddInput(&_axisRightX);
		AddInput(&_axisRightY);

		// Buttons
		AddInput(&_buttonA);
		AddInput(&_buttonB);
		AddInput(&_buttonY);
		AddInput(&_buttonX);

		AddInput(&_buttonDpadUp);
		AddInput(&_buttonDpadDown);
		AddInput(&_buttonDpadLeft);
		AddInput(&_buttonDpadRight);

		AddInput(&_buttonLB);
		AddInput(&_buttonLT);
		AddInput(&_buttonLS);

		AddInput(&_buttonRB);
		AddInput(&_buttonRT);
		AddInput(&_buttonRS);

		AddInput(&_buttonStart);
		AddInput(&_buttonBack);
		*/
	}
}
