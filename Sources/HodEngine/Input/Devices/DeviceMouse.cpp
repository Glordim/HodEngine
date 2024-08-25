#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/DeviceMouse.hpp"

namespace hod::input
{
	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	DeviceMouse::DeviceMouse(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::MOUSE, uid, name, product)
	{

	}
}
