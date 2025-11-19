#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"

#include <cassert>

namespace hod
{
	/// @brief
	/// @param type
	/// @param offset
	/// @param name
	ReflectionPropertyArray::ReflectionPropertyArray(Adapter adapter, ReflectionPropertyVariable::Type type, uint32_t offset, const char* name,
	                                                 ReflectionDescriptor* elementReflectionDescriptor)
	: ReflectionProperty(offset, name)
	, _type(type)
	, _elementReflectionDescriptor(elementReflectionDescriptor)
	, _adapter(adapter)
	{
	}

	/// @brief
	/// @return
	ReflectionPropertyVariable::Type ReflectionPropertyArray::GetType() const
	{
		return _type;
	}

	ReflectionDescriptor* ReflectionPropertyArray::GetElementReflectionDescriptor() const
	{
		return _elementReflectionDescriptor;
	}

	uint32_t ReflectionPropertyArray::GetElementCount(const void* instance) const
	{
		const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
		const void*    arrayAddress = reinterpret_cast<const void*>(instanceAddress + _offset);

		return _adapter._getElementCountFunction(arrayAddress);
	}

	void ReflectionPropertyArray::InsertElement(void* instance, uint32_t index) const
	{
		if (_adapter._insertElementFunction != nullptr)
		{
			uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
			void*    arrayAddress = reinterpret_cast<void*>(instanceAddress + _offset);

			_adapter._insertElementFunction(arrayAddress, index);
		}
	}

	void ReflectionPropertyArray::RemoveElement(void* instance, uint32_t index) const
	{
		if (_adapter._removeElementFunction != nullptr)
		{
			uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
			void*    arrayAddress = reinterpret_cast<void*>(instanceAddress + _offset);

			_adapter._removeElementFunction(arrayAddress, index);
		}
	}

	void ReflectionPropertyArray::Clear(void* instance) const
	{
		if (_adapter._clearFunction != nullptr)
		{
			uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
			void*    arrayAddress = reinterpret_cast<void*>(instanceAddress + _offset);

			_adapter._clearFunction(arrayAddress);
		}
	}

	/// @brief
	/// @param sourceInstance
	/// @param destinationInstance
	void ReflectionPropertyArray::Copy(const void* sourceInstance, void* destinationInstance) const
	{
		// TODO
		(void)sourceInstance;
		(void)destinationInstance;
	}
}
