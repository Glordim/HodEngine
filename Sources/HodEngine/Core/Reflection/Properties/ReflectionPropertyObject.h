#pragma once

#include <vector>
#include <functional>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"

namespace hod
{
	///@brief 
	class ReflectionPropertyObject : public ReflectionProperty // todo rename remove namespace
	{
		META_TYPE(ReflectionPropertyObject, ReflectionProperty)

	public:

												ReflectionPropertyObject(uint32_t offset, const char* name, ReflectionDescriptor* reflectionDesceriptor, std::function<void(void*, void*)> setMethod, std::function<void*(const void*)> getMethod);
												ReflectionPropertyObject(const ReflectionPropertyObject&) = default;
												ReflectionPropertyObject(ReflectionPropertyObject&&) = default;
												~ReflectionPropertyObject() = default;

		ReflectionPropertyObject&				operator = (const ReflectionPropertyObject&) = default;
		ReflectionPropertyObject&				operator = (ReflectionPropertyObject&&) = default;

	public:

		void*									GetInstance(void* instance) const;

		void*									GetValue(const void* instance) const;
		void									SetValue(void* instance, void* value);

		ReflectionDescriptor*					GetReflectionDescriptor() const;

	private:

		ReflectionDescriptor*					_reflectionDesceriptor;
		std::function<void(void*, void*)>		_setMethod;
		std::function<void* (const void*)>		_getMethod;
	};
}
