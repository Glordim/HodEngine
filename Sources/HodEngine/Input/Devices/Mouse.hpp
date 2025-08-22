#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Device.hpp"
#include "HodEngine/Input/State.hpp"

#include "HodEngine/Input/InputButton.hpp"
#include "HodEngine/Input/InputAxis.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API Mouse : public Device
	{
	public:

						Mouse(const UID& uid, const std::string_view& name, Product product);
						Mouse(const Mouse&) = delete;
						Mouse(Mouse&&) = delete;
						~Mouse() override = default;

		Mouse&			operator = (const Mouse&) = delete;
		Mouse&			operator = (Mouse&&) = delete;

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
