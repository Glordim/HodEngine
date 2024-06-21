#include "HodEngine/Input/Devices/DevicePad.hpp"

namespace hod::input
{
	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	DevicePad::DevicePad(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::PAD, uid, name, product)
	{

	}
}
