#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

namespace hod
{
	/// @brief 
	/// @param offset 
	/// @param name 
	ReflectionPropertyObject::ReflectionPropertyObject(uint32_t offset, const char* name, ReflectionDescriptor* reflectionDesceriptor, std::function<void(void*, void*)> setMethod, std::function<void*(const void*)> getMethod)
		: ReflectionProperty(offset, name)
		, _reflectionDescriptor(reflectionDesceriptor)
		, _setMethod(setMethod)
		, _getMethod(getMethod)
	{

	}

	/// @brief 
	/// @param instance 
	/// @return 
	void* ReflectionPropertyObject::GetInstance(void* instance) const // todo use it on deserialize
	{
		uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
		uint8_t* objectAddress = instanceAddress + _offset;

		return reinterpret_cast<void*>(objectAddress);
	}

	/// @brief 
	/// @param instance 
	/// @return 
	const void* ReflectionPropertyObject::GetInstance(const void* instance) const
	{
		const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
		const uint8_t* objectAddress = instanceAddress + _offset;

		return reinterpret_cast<const void*>(objectAddress);
	}

	/// @brief 
	/// @param instance 
	/// @return 
	void* ReflectionPropertyObject::GetValue(const void* instance) const
	{
		if (_getMethod != nullptr)
		{
			return _getMethod(instance);
		}

		const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
		const uint8_t* objectAddress = instanceAddress + _offset;

		return (void*)reinterpret_cast<const void*>(objectAddress);
	}

	/// @brief 
	/// @param instance 
	/// @param value 
	void ReflectionPropertyObject::SetValue(void* instance, void* value)
	{
		if (_setMethod != nullptr)
		{
			_setMethod(instance, value);
		}
	}

	/// @brief 
	/// @return 
	ReflectionDescriptor* ReflectionPropertyObject::GetReflectionDescriptor() const
	{
		return _reflectionDescriptor;
	}

	/// @brief 
	/// @param sourceInstance 
	/// @param destinationInstance 
	void ReflectionPropertyObject::Copy(const void* sourceInstance, void* destinationInstance)
	{
		_reflectionDescriptor->Copy(GetInstance(sourceInstance), GetInstance(destinationInstance));
	}
}
