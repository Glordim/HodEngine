
namespace hod
{
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
		else if constexpr (std::is_same<_MemberVariable_, std::string>::value)
		{
			return ReflectionPropertyVariable::Type::String;
		}
		else
		{
			[]<bool flag = false>()
			{
				static_assert(flag, "no match");
			}();
		}
	}

	template<typename _MemberVariable_>
	ReflectionProperty* ReflectionHelper::AddProperty(ReflectionDescriptor* descriptor, const std::string_view& name, uint32_t offset, std::function<void(void*, void*)> setMethod, std::function<void*(const void*)> getMethod)
	{
		//static_assert(std::is_member_object_pointer<T>::value

		if constexpr (std::is_array<_MemberVariable_>::value) // todo support std::vector
		{
			return descriptor->AddProperty<ReflectionPropertyArray>(GetVariableType<_MemberVariable_>(), offset, name.data()); // TODO remove data, descriptor must use string view
		}
		else if constexpr (std::is_arithmetic<_MemberVariable_>::value || std::is_same<_MemberVariable_, std::string>::value)
		{
			return descriptor->AddProperty<ReflectionPropertyVariable>(GetVariableType<_MemberVariable_>(), offset, name.data(), setMethod, getMethod); // TODO remove data, descriptor must use string view
		}
		else if constexpr (std::is_class<_MemberVariable_>::value)
		{
			return descriptor->AddProperty<ReflectionPropertyObject>(offset, name.data(), _MemberVariable_::GetReflectionDescriptor(), setMethod, getMethod); // TODO remove data, descriptor must use string view
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
