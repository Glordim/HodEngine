#pragma once

#include "HodEngine/Input/Device.hpp"

namespace hod::input
{
	/// @brief 
	class DeviceMouse : public Device
	{
	public:

						DeviceMouse(const UID& uid, const std::string_view& name, Product product);
						DeviceMouse(const DeviceMouse&) = delete;
						DeviceMouse(DeviceMouse&&) = delete;
						~DeviceMouse() override = default;

		DeviceMouse&	operator = (const DeviceMouse&) = delete;
		DeviceMouse&	operator = (DeviceMouse&&) = delete;
	};
}
