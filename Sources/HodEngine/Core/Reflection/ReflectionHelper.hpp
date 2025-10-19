#pragma once
#include "HodEngine/Core/Export.hpp"

#include <functional>
#include <string_view>
#include <type_traits>

#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/TypeTrait.hpp"

namespace hod
{
	///@brief
	class HOD_CORE_API ReflectionHelper
	{
	public:
		template<typename _MemberVariable_>
		static ReflectionProperty* AddProperty(ReflectionDescriptor& descriptor, const std::string_view& name, uint32_t offset, std::function<void(void*, void*)> = nullptr,
		                                       std::function<void*(const void*)> = nullptr);
	};
}

namespace hod
{
	template<typename _Enum_>
	class ReflectedEnum
	{
	public:
		static EnumDescriptor& GetEnumDescriptor()
		{
			static EnumDescriptor descriptor = []()
			{
				EnumDescriptor desc;
				DescribeEnum(desc, _Enum_ {}); // ADL (you must use DESCRIBE_REFLECTED_ENUM macro to use ReflectedEnum on it)
				return desc;
			}();
			return descriptor;
		}
	};

	template<typename _Class_>
	class ReflectedClass
	{
	public:
		static ReflectionDescriptor& GetReflectionDescriptor()
		{
			static ReflectionDescriptor reflectionDescriptor;
			static bool                 init = false;
			if (init == false)
			{
				_Class_::FillReflectionDescriptor(reflectionDescriptor);
				init = true;
			}
			return reflectionDescriptor;
		}
	};

	// std::function<void(T::*)(const MemberType&)> setFunction = nullptr

	template<typename T, typename MemberType>
		requires(std::is_fundamental_v<MemberType> || std::is_enum_v<MemberType>)
	ReflectionProperty* AddPropertyT(ReflectionDescriptor& descriptor, MemberType T::* member, const char* name,
	                                 void (T::*setFunction)(MemberType) = nullptr /*, std::function<MemberType(void) const> getFunction = nullptr*/)
	{
		uint32_t offset = OffsetOf(member);

		if (setFunction != nullptr)
		{
			ReflectionProperty* property = ReflectionHelper::AddProperty<MemberType>(
				descriptor, name, offset,
				[setFunction](void* instance, void* value)
				{
					T*         instanceClass = static_cast<T*>(instance);
					MemberType valueCopy = *static_cast<MemberType*>(value);
					(instanceClass->*setFunction)(valueCopy);
				},
				nullptr);

			return property;
		}
		else
		{
			return ReflectionHelper::AddProperty<MemberType>(descriptor, name, offset);
		}
	}

	template<typename T, typename MemberType>
		requires(!std::is_fundamental_v<MemberType> && !std::is_enum_v<MemberType>)
	ReflectionProperty* AddPropertyT(ReflectionDescriptor& descriptor, MemberType T::* member, const char* name,
	                                 void (T::*setFunction)(const MemberType&) = nullptr /*, std::function<const MemberType&(void) const> getFunction = nullptr*/)
	{
		uint32_t offset = OffsetOf(member);
		if (setFunction != nullptr)
		{
			ReflectionProperty* property = ReflectionHelper::AddProperty<MemberType>(
				descriptor, name, offset,
				[setFunction](void* instance, void* value)
				{
					T*                instanceClass = static_cast<T*>(instance);
					const MemberType& valueRef = *static_cast<MemberType*>(value);
					(instanceClass->*setFunction)(valueRef);
				},
				nullptr);
			return property;
		}
		else
		{
			return ReflectionHelper::AddProperty<MemberType>(descriptor, name, offset);
		}
	}
}

#include "ReflectionHelper.inl"
