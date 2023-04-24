
namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				template<typename T>
				void Variable::Serialize(const T& instance, Document::Element& element)
				{
					const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(&instance);
					const uint8_t* variableAddress = instanceAddress + _offset;
					
					switch (_type)
					{
					case Type::Bool: element.SetBool(_name, *reinterpret_cast<const bool*>(variableAddress)); break;
					case Type::Int8: element.SetInt8(_name, *reinterpret_cast<const int8_t*>(variableAddress)); break;
					case Type::Int16: element.SetInt16(_name, *reinterpret_cast<const int16_t*>(variableAddress)); break;
					case Type::Int32: element.SetInt32(_name, *reinterpret_cast<const int32_t*>(variableAddress)); break;
					case Type::Int64: element.SetInt64(_name, *reinterpret_cast<const int64_t*>(variableAddress)); break;
					case Type::UInt8: element.SetUInt8(_name, *reinterpret_cast<const uint8_t*>(variableAddress)); break;
					case Type::UInt16: element.SetUInt16(_name, *reinterpret_cast<const uint16_t*>(variableAddress)); break;
					case Type::UInt32: element.SetUInt32(_name, *reinterpret_cast<const uint32_t*>(variableAddress)); break;
					case Type::UInt64: element.SetUInt64(_name, *reinterpret_cast<const uint64_t*>(variableAddress)); break;
					case Type::Float32: element.SetFloat32(_name, *reinterpret_cast<const float*>(variableAddress)); break;
					case Type::Float64: element.SetFloat64(_name, *reinterpret_cast<const double*>(variableAddress)); break;
					}
				}

				template<typename T>
				void Variable::Deserialize(T& instance, const Document::Element& element)
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(&instance);
					uint8_t* variableAddress = instanceAddress + _offset;

					switch (_type)
					{
					case Type::Bool: *reinterpret_cast<bool*>(variableAddress) = element.GetBool(_name, *reinterpret_cast<bool*>(variableAddress)); break;
					case Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = element.GetInt8(_name, *reinterpret_cast<int8_t*>(variableAddress)); break;
					case Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = element.GetInt16(_name, *reinterpret_cast<int16_t*>(variableAddress)); break;
					case Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = element.GetInt32(_name, *reinterpret_cast<int32_t*>(variableAddress)); break;
					case Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = element.GetInt64(_name, *reinterpret_cast<int64_t*>(variableAddress)); break;
					case Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = element.GetUInt8(_name, *reinterpret_cast<uint8_t*>(variableAddress)); break;
					case Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = element.GetUInt16(_name, *reinterpret_cast<uint16_t*>(variableAddress)); break;
					case Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = element.GetUInt32(_name, *reinterpret_cast<uint32_t*>(variableAddress)); break;
					case Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = element.GetUInt64(_name, *reinterpret_cast<uint64_t*>(variableAddress)); break;
					case Type::Float32: *reinterpret_cast<float*>(variableAddress) = element.GetFloat32(_name, *reinterpret_cast<float*>(variableAddress)); break;
					case Type::Float64: *reinterpret_cast<double*>(variableAddress) = element.GetFloat64(_name, *reinterpret_cast<double*>(variableAddress)); break;
					}
				}
			}
		}
	}
}
