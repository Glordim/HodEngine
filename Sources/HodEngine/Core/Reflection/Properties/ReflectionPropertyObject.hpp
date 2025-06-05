#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <functional>

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

namespace hod
{
	///@brief 
	class HOD_CORE_API ReflectionPropertyObject : public ReflectionProperty // todo rename remove namespace
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
		const void*								GetInstance(const void* instance) const;

		void*									GetValue(const void* instance) const;
		void									SetValue(void* instance, const void* value) const;

		ReflectionDescriptor*					GetReflectionDescriptor() const;

		void									Copy(const void* sourceInstance, void* destinationInstance) const override;
		bool									CompareInstance(const void* left, const void* right) const;

		bool									HasSetMethod() const;

	private:

		ReflectionDescriptor*					_reflectionDescriptor = nullptr;
		std::function<void(void*, void*)>		_setMethod;
		std::function<void* (const void*)>		_getMethod;
	};
}
