#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Input/Device.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_API DevicePad : public Device
	{
	public:

						DevicePad(const UID& uid, const std::string_view& name, Product product);
						DevicePad(const DevicePad&) = delete;
						DevicePad(DevicePad&&) = delete;
						~DevicePad() override = default;

		DevicePad&		operator = (const DevicePad&) = delete;
		DevicePad&		operator = (DevicePad&&) = delete;
	};
}
