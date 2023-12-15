#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"

#include <cassert>

namespace hod
{
	/// @brief 
	/// @param type 
	/// @param offset 
	/// @param name 
	ReflectionPropertyArray::ReflectionPropertyArray(ReflectionPropertyVariable::Type type, uint32_t offset, const char* name)
		: ReflectionProperty(offset, name)
		, _type(type)
	{
		_getElementAddressFunction = &ReflectionPropertyArray::GetElementAddress;
	}

	/// @brief 
	/// @param instance 
	/// @param index 
	/// @return 
	void* ReflectionPropertyArray::GetElementAddress(void* instance, uint32_t index)
	{
		uint8_t* address = reinterpret_cast<uint8_t*>(instance) + index;
		return reinterpret_cast<void*>(address);
	}

	/// @brief 
	/// @return 
	ReflectionPropertyVariable::Type ReflectionPropertyArray::GetType() const
	{
		return _type;
	}
}
