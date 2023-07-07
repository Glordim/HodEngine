
namespace hod
{
	template<typename _MemberVariable_>
	constexpr Reflection::Property::Variable::Type GetVariableType()
	{
		if constexpr (std::is_same<_MemberVariable_, bool>::value)
		{
			return Reflection::Property::Variable::Type::Bool;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint8_t>::value)
		{
			return Reflection::Property::Variable::Type::UInt8;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint16_t>::value)
		{
			return Reflection::Property::Variable::Type::UInt16;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint32_t>::value)
		{
			return Reflection::Property::Variable::Type::UInt32;
		}
		else if constexpr (std::is_same<_MemberVariable_, uint64_t>::value)
		{
			return Reflection::Property::Variable::Type::UInt64;
		}
		else if constexpr (std::is_same<_MemberVariable_, int8_t>::value)
		{
			return Reflection::Property::Variable::Type::Int8;
		}
		else if constexpr (std::is_same<_MemberVariable_, int16_t>::value)
		{
			return Reflection::Property::Variable::Type::Int16;
		}
		else if constexpr (std::is_same<_MemberVariable_, int32_t>::value)
		{
			return Reflection::Property::Variable::Type::Int32;
		}
		else if constexpr (std::is_same<_MemberVariable_, int64_t>::value)
		{
			return Reflection::Property::Variable::Type::Int64;
		}
		else if constexpr (std::is_same<_MemberVariable_, float>::value)
		{
			return Reflection::Property::Variable::Type::Float32;
		}
		else if constexpr (std::is_same<_MemberVariable_, double>::value)
		{
			return Reflection::Property::Variable::Type::Float64;
		}
		else if constexpr (std::is_same<_MemberVariable_, std::string>::value)
		{
			return Reflection::Property::Variable::Type::String;
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
	ReflectionProperty* ReflectionHelper::AddProperty(ReflectionDescriptor* descriptor, const std::string_view& name, uint32_t offset)
	{
		//static_assert(std::is_member_object_pointer<T>::value

		if constexpr (std::is_array<_MemberVariable_>::value) // todo support std::vector
		{
			return descriptor->AddProperty<Reflection::Property::Array>(GetVariableType<_MemberVariable_>(), offset, name.data()); // TODO remove data, descriptor must use string view
		}
		else if constexpr (std::is_arithmetic<_MemberVariable_>::value || std::is_same<_MemberVariable_, std::string>::value)
		{
			return descriptor->AddProperty<Reflection::Property::Variable>(GetVariableType<_MemberVariable_>(), offset, name.data()); // TODO remove data, descriptor must use string view
		}
		else if constexpr (std::is_class<_MemberVariable_>::value)
		{
			return descriptor->AddProperty<Reflection::Property::Object>(offset, name.data(), _MemberVariable_::GetReflectionDescriptor()); // TODO remove data, descriptor must use string view
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
