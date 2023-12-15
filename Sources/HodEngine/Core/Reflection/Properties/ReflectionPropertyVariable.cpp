#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"

#include <cassert>

namespace hod
{
	/// @brief 
	/// @param type 
	/// @param offset 
	/// @param name 
	/// @param setMethod 
	/// @param getMethod 
	ReflectionPropertyVariable::ReflectionPropertyVariable(Type type, uint32_t offset, const char* name, std::function<void(void*, void*)> setMethod, std::function<void*(const void*)> getMethod)
		: ReflectionProperty(offset, name)
		, _type(type)
		, _setMethod(setMethod)
		, _getMethod(getMethod)
	{

	}

	/// @brief 
	/// @return 
	ReflectionPropertyVariable::Type ReflectionPropertyVariable::GetType() const
	{
		return _type;
	}
}
