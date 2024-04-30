
namespace hod
{
	template<typename _type_>
	_type_ ReflectionPropertyVariable::GetValue(const void* instance) const
	{
		const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
		const uint8_t* variableAddress = instanceAddress + _offset;

		if (_getMethod != nullptr)
		{
			//return reinterpret_cast<_type_>(_getMethod(instance));
			return _type_();
		}
		else
		{
			if constexpr (std::is_same_v<_type_, std::string> == true)
			{
				return *reinterpret_cast<const std::string*>(variableAddress);
			}
			else
			{
				switch (_type)
				{
				case Type::Bool: return static_cast<_type_>(*reinterpret_cast<const bool*>(variableAddress)); break;
				case Type::Int8: return static_cast<_type_>(*reinterpret_cast<const int8_t*>(variableAddress)); break;
				case Type::Int16: return static_cast<_type_>(*reinterpret_cast<const int16_t*>(variableAddress)); break;
				case Type::Int32: return static_cast<_type_>(*reinterpret_cast<const int32_t*>(variableAddress)); break;
				case Type::Int64: return static_cast<_type_>(*reinterpret_cast<const int64_t*>(variableAddress)); break;
				case Type::UInt8: return static_cast<_type_>(*reinterpret_cast<const uint8_t*>(variableAddress)); break;
				case Type::UInt16: return static_cast<_type_>(*reinterpret_cast<const uint16_t*>(variableAddress)); break;
				case Type::UInt32: return static_cast<_type_>(*reinterpret_cast<const uint32_t*>(variableAddress)); break;
				case Type::UInt64: return static_cast<_type_>(*reinterpret_cast<const uint64_t*>(variableAddress)); break;
				case Type::Float32: return static_cast<_type_>(*reinterpret_cast<const float*>(variableAddress)); break;
				case Type::Float64: return static_cast<_type_>(*reinterpret_cast<const double*>(variableAddress)); break;
				default: assert(false); break;
				}
			}
		}

		return _type_();
	}

	template<typename _type_>
	void ReflectionPropertyVariable::SetValue(void* instance, _type_ value) const
	{
		uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
		uint8_t* variableAddress = instanceAddress + _offset;

		if (_setMethod != nullptr)
		{
			_setMethod(instance, reinterpret_cast<void*>(&value));
		}
		else
		{
			if constexpr (std::is_same_v<_type_, std::string> == true)
			{
				*reinterpret_cast<std::string*>(variableAddress) = value;
			}
			else
			{
				switch (_type)
				{
				case Type::Bool: *reinterpret_cast<bool*>(variableAddress) = static_cast<bool>(value); break;
				case Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = static_cast<int8_t>(value); break;
				case Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = static_cast<int16_t>(value); break;
				case Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = static_cast<int32_t>(value); break;
				case Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = static_cast<int64_t>(value); break;
				case Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = static_cast<uint8_t>(value); break;
				case Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = static_cast<uint16_t>(value); break;
				case Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = static_cast<uint32_t>(value); break;
				case Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = static_cast<uint64_t>(value); break;
				case Type::Float32: *reinterpret_cast<float*>(variableAddress) = static_cast<float>(value); break;
				case Type::Float64: *reinterpret_cast<double*>(variableAddress) = static_cast<double>(value); break;
				default: assert(false); break;
				}
			}
		}
	}
}
