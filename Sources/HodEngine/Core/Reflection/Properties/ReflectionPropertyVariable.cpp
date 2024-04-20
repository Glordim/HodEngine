#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"

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

	/// @brief 
	/// @param sourceInstance 
	/// @param destinationInstance 
	void ReflectionPropertyVariable::Copy(const void* sourceInstance, void* destinationInstance)
	{
		switch (_type)
		{
		case Type::Bool: SetValue<bool>(destinationInstance, GetValue<bool>(sourceInstance)); break;
		case Type::Int8: SetValue<int8_t>(destinationInstance, GetValue<int8_t>(sourceInstance)); break;
		case Type::Int16: SetValue<int16_t>(destinationInstance, GetValue<int16_t>(sourceInstance)); break;
		case Type::Int32: SetValue<int32_t>(destinationInstance, GetValue<int32_t>(sourceInstance)); break;
		case Type::Int64: SetValue<int64_t>(destinationInstance, GetValue<int64_t>(sourceInstance)); break;
		case Type::UInt8: SetValue<uint8_t>(destinationInstance, GetValue<uint8_t>(sourceInstance)); break;
		case Type::UInt16: SetValue<uint16_t>(destinationInstance, GetValue<uint16_t>(sourceInstance)); break;
		case Type::UInt32: SetValue<uint32_t>(destinationInstance, GetValue<uint32_t>(sourceInstance)); break;
		case Type::UInt64: SetValue<uint64_t>(destinationInstance, GetValue<uint64_t>(sourceInstance)); break;
		case Type::Float32: SetValue<float>(destinationInstance, GetValue<float>(sourceInstance)); break;
		case Type::Float64: SetValue<double>(destinationInstance, GetValue<double>(sourceInstance)); break;
		//case Type::String: SetValue<bool>(destinationInstance, GetValue<bool>(sourceInstance)); break;

		default: assert(false); break;
		}
	}
}
