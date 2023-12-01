
namespace hod
{
	namespace Reflection
	{
		namespace Property
		{
			template<typename _type_>
			_type_ Variable::GetValue(const void* instance) const
			{
				const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
				const uint8_t* variableAddress = instanceAddress + _offset;

				switch (_type)
				{
				case Type::Bool: return *reinterpret_cast<const bool*>(variableAddress); break;
				case Type::Int8: return *reinterpret_cast<const int8_t*>(variableAddress); break;
				case Type::Int16: return *reinterpret_cast<const int16_t*>(variableAddress); break;
				case Type::Int32: return *reinterpret_cast<const int32_t*>(variableAddress); break;
				case Type::Int64: return *reinterpret_cast<const int64_t*>(variableAddress); break;
				case Type::UInt8: return *reinterpret_cast<const uint8_t*>(variableAddress); break;
				case Type::UInt16: return *reinterpret_cast<const uint16_t*>(variableAddress); break;
				case Type::UInt32: return *reinterpret_cast<const uint32_t*>(variableAddress); break;
				case Type::UInt64: return *reinterpret_cast<const uint64_t*>(variableAddress); break;
				case Type::Float32: return *reinterpret_cast<const float*>(variableAddress); break;
				case Type::Float64: return *reinterpret_cast<const double*>(variableAddress); break;
				//case Type::String: return *reinterpret_cast<const std::string*>(variableAddress); break;

				default: assert(false); break;
				}

				return _type_();
			}

			template<typename _type_>
			void Variable::SetValue(void* instance, _type_ value)
			{
				uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
				uint8_t* variableAddress = instanceAddress + _offset;

				if (_setMethod != nullptr)
				{
					_setMethod(instance, reinterpret_cast<void*>(&value));
				}
				else
				{
					switch (_type)
					{
					case Type::Bool: *reinterpret_cast<bool*>(variableAddress) = value; break;
					case Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = value; break;
					case Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = value; break;
					case Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = value; break;
					case Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = value; break;
					case Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = value; break;
					case Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = value; break;
					case Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = value; break;
					case Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = value; break;
					case Type::Float32: *reinterpret_cast<float*>(variableAddress) = value; break;
					case Type::Float64: *reinterpret_cast<double*>(variableAddress) = value; break;
					//case Type::String: *reinterpret_cast<std::string*>(variableAddress) = value; break;

					default: assert(false); break;
					}
				}
			}
		}
	}
}
