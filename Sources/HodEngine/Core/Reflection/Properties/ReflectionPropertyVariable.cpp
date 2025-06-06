#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"

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
	ReflectionPropertyVariable::~ReflectionPropertyVariable()
	{
		DefaultAllocator::GetInstance().Delete(_enumDescriptor);
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
	void ReflectionPropertyVariable::Copy(const void* sourceInstance, void* destinationInstance) const
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
		case Type::String: SetValue<std::string>(destinationInstance, GetValue<std::string>(sourceInstance)); break;

		default: assert(false); break;
		}
	}

	/// @brief 
	/// @param left 
	/// @param right 
	/// @return 
	bool ReflectionPropertyVariable::CompareInstance(const void* left, const void* right) const
	{
		switch (_type)
		{
		case Type::Bool: return (GetValue<bool>(left) == GetValue<bool>(right));
		case Type::Int8: return (GetValue<int8_t>(left) == GetValue<int8_t>(right));
		case Type::Int16: return (GetValue<int16_t>(left) == GetValue<int16_t>(right));
		case Type::Int32: return (GetValue<int32_t>(left) == GetValue<int32_t>(right));
		case Type::Int64: return (GetValue<int64_t>(left) == GetValue<int64_t>(right));
		case Type::UInt8: return (GetValue<uint8_t>(left) == GetValue<uint8_t>(right));
		case Type::UInt16: return (GetValue<uint16_t>(left) == GetValue<uint16_t>(right));
		case Type::UInt32: return (GetValue<uint32_t>(left) == GetValue<uint32_t>(right));
		case Type::UInt64: return (GetValue<uint64_t>(left) == GetValue<uint64_t>(right));
		case Type::Float32: return (GetValue<float>(left) == GetValue<float>(right));
		case Type::Float64: return (GetValue<double>(left) == GetValue<double>(right));
		case Type::String: return (GetValue<std::string>(left) == GetValue<std::string>(right));

		default: assert(false); return false;
		}
	}

	/// @brief 
	/// @param enumDescriptor 
	void ReflectionPropertyVariable::SetEnumDescriptor(EnumDescriptor* enumDescriptor)
	{
		_enumDescriptor = enumDescriptor;
	}

	/// @brief 
	/// @return 
	EnumDescriptor* ReflectionPropertyVariable::GetEnumDescriptor() const
	{
		return _enumDescriptor;
	}
}
