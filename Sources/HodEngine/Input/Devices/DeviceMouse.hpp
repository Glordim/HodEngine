#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Device.hpp"
#include "HodEngine/Input/State.hpp"

#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/InputAxis.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DeviceMouse : public Device
	{
	public:

						DeviceMouse(const UID& uid, const std::string_view& name, Product product);
						DeviceMouse(const DeviceMouse&) = delete;
						DeviceMouse(DeviceMouse&&) = delete;
						~DeviceMouse() override = default;

		DeviceMouse&	operator = (const DeviceMouse&) = delete;
		DeviceMouse&	operator = (DeviceMouse&&) = delete;

	private:

		//InputAxis		_axis;
		InputAxis		_axisX;
		InputAxis		_axisY;

		InputAxis		_axisWheel;

		InputButton		_buttonLeft;
		InputButton		_buttonRight;
		InputButton		_buttonWheel;
		InputButton		_button4;
		InputButton		_button5;
	};
}
