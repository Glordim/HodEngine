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
#include "HodEngine/Core/TypeTrait.hpp"

namespace hod::inline core
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

namespace hod::inline core
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
				if constexpr (requires { _Class_::FillReflectionDescriptor(reflectionDescriptor); })
				{
					_Class_::FillReflectionDescriptor(reflectionDescriptor);
				}
				else
				{
					DescribeClass(reflectionDescriptor, (const _Class_*)nullptr); // ADL
				}
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

	template<typename _MemberVariable_>
	constexpr ReflectionPropertyVariable::Type GetVariableType()
	{
		if constexpr (std::is_same<_MemberVariable_, bool>::value)
		{
			return ReflectionPropertyVariable::Type::Bool;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint8_t>::value)
		{
			return ReflectionPropertyVariable::Type::UInt8;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint16_t>::value)
		{
			return ReflectionPropertyVariable::Type::UInt16;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint32_t>::value)
		{
			return ReflectionPropertyVariable::Type::UInt32;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint64_t>::value)
		{
			return ReflectionPropertyVariable::Type::UInt64;
		}
		else if constexpr (std::is_same<_MemberVariable_, int8_t>::value)
		{
			return ReflectionPropertyVariable::Type::Int8;
		}
		else if constexpr (std::is_same<_MemberVariable_, int16_t>::value)
		{
			return ReflectionPropertyVariable::Type::Int16;
		}
		else if constexpr (std::is_same<_MemberVariable_, int32_t>::value)
		{
			return ReflectionPropertyVariable::Type::Int32;
		}
		else if constexpr (std::is_same<_MemberVariable_, int64_t>::value)
		{
			return ReflectionPropertyVariable::Type::Int64;
		}
		else if constexpr (std::is_same<_MemberVariable_, float>::value)
		{
			return ReflectionPropertyVariable::Type::Float32;
		}
		else if constexpr (std::is_same<_MemberVariable_, double>::value)
		{
			return ReflectionPropertyVariable::Type::Float64;
		}
		else if constexpr (std::is_same<_MemberVariable_, String>::value)
		{
			return ReflectionPropertyVariable::Type::String;
		}
		else if constexpr (std::is_class<_MemberVariable_>::value)
		{
			return ReflectionPropertyVariable::Type::Object;
		}
		else if constexpr (std::is_pointer<_MemberVariable_>::value)
		{
			return ReflectionPropertyVariable::Type::Object;
		}
		else if constexpr (std::is_enum<_MemberVariable_>::value)
		{
			return GetVariableType<typename std::underlying_type<_MemberVariable_>::type>();
		}
		else
		{
			static_assert(always_false<_MemberVariable_>);
		}
	}

	template<typename _MemberVariable_>
	ReflectionProperty* ReflectionHelper::AddProperty(ReflectionDescriptor& descriptor, const std::string_view& name, uint32_t offset, std::function<void(void*, void*)> setMethod,
	                                                  std::function<void*(const void*)> getMethod)
	{
		// static_assert(std::is_member_object_pointer<T>::value

		if constexpr (std::is_array<_MemberVariable_>::value)
		{
			ReflectionPropertyVariable::Type type = GetVariableType<_MemberVariable_>();
			return descriptor.AddProperty<ReflectionPropertyArray>(MakeAdapter<_MemberVariable_>(), GetVariableType<_MemberVariable_>(), offset, name.data(),
			                                                       type == ReflectionPropertyVariable::Type::Object ? _MemberVariable_::GetReflectionDescriptor()
			                                                                                                        : nullptr); // TODO remove data, descriptor must use string view
		}
		else if constexpr (IsVector<_MemberVariable_>::value)
		{
			constexpr ReflectionPropertyVariable::Type type = GetVariableType<typename _MemberVariable_::value_type>();
			if constexpr (type == ReflectionPropertyVariable::Type::Object)
			{
				ReflectionDescriptor* valueTypeDescriptor = nullptr;
				if constexpr (std::is_pointer_v<typename _MemberVariable_::value_type>)
				{
					valueTypeDescriptor = &std::remove_pointer_t<typename _MemberVariable_::value_type>::GetReflectionDescriptor();
				}
				else
				{
					valueTypeDescriptor = &_MemberVariable_::value_type::GetReflectionDescriptor();
				}
				return descriptor.AddProperty<ReflectionPropertyArray>(MakeAdapter<_MemberVariable_>(), type, offset, name.data(),
				                                                       valueTypeDescriptor); // TODO remove data, descriptor must use string view
			}
			else
			{
				return descriptor.AddProperty<ReflectionPropertyArray>(MakeAdapter<_MemberVariable_>(), type, offset, name.data(), nullptr);
			}
		}
		else if constexpr (std::is_arithmetic<_MemberVariable_>::value || std::is_same<_MemberVariable_, String>::value || std::is_enum<_MemberVariable_>::value)
		{
			ReflectionPropertyVariable* propertyVariable = descriptor.AddProperty<ReflectionPropertyVariable>(GetVariableType<_MemberVariable_>(), offset, name.data(), setMethod,
			                                                                                                  getMethod); // TODO remove data, descriptor must use string view
			if constexpr (std::is_enum<_MemberVariable_>::value)
			{
				propertyVariable->SetEnumDescriptor(&ReflectedEnum<_MemberVariable_>::GetEnumDescriptor());
			}
			return propertyVariable;
		}
		else if constexpr (std::is_class<_MemberVariable_>::value)
		{
			return descriptor.AddProperty<ReflectionPropertyObject>(offset, name.data(), &ReflectedClass<_MemberVariable_>::GetReflectionDescriptor(), setMethod,
			                                                        getMethod); // TODO remove data, descriptor must use string view
		}
		else
		{
			/*
			[]<bool flag = false>()
			{
			    static_assert(flag, "no match");
			}();
			*/
			return nullptr;
		}
	}
}
