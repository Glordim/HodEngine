
namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				template<typename T>
				void Variable::Serialize(const T& instance, Document::Node& node)
				{
					const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(&instance);
					const uint8_t* variableAddress = instanceAddress + _offset;

					Document::Node& varNode = node.GetOrAddChild(_name);
					
					switch (_type)
					{
					case Type::Bool: node.SetBool(*reinterpret_cast<const bool*>(variableAddress)); break;
					case Type::Int8: node.SetInt8(*reinterpret_cast<const int8_t*>(variableAddress)); break;
					case Type::Int16: node.SetInt16(*reinterpret_cast<const int16_t*>(variableAddress)); break;
					case Type::Int32: node.SetInt32(*reinterpret_cast<const int32_t*>(variableAddress)); break;
					case Type::Int64: node.SetInt64(*reinterpret_cast<const int64_t*>(variableAddress)); break;
					case Type::UInt8: node.SetUInt8(*reinterpret_cast<const uint8_t*>(variableAddress)); break;
					case Type::UInt16: node.SetUInt16(*reinterpret_cast<const uint16_t*>(variableAddress)); break;
					case Type::UInt32: node.SetUInt32(*reinterpret_cast<const uint32_t*>(variableAddress)); break;
					case Type::UInt64: node.SetUInt64(*reinterpret_cast<const uint64_t*>(variableAddress)); break;
					case Type::Float32: node.SetFloat32(*reinterpret_cast<const float*>(variableAddress)); break;
					case Type::Float64: node.SetFloat64(*reinterpret_cast<const double*>(variableAddress)); break;
					}
				}

				template<typename T>
				void Variable::Deserialize(T& instance, const Document::Node& node)
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(&instance);
					uint8_t* variableAddress = instanceAddress + _offset;

					const Document::Node* varNode = node.GetChild(_name);
					if (varNode != nullptr)
					{
						switch (_type)
						{
						case Type::Bool: *reinterpret_cast<bool*>(variableAddress) = node.GetBool(); break;
						case Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = node.GetInt8(); break;
						case Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = node.GetInt16(); break;
						case Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = node.GetInt32(); break;
						case Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = node.GetInt64(); break;
						case Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = node.GetUInt8(); break;
						case Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = node.GetUInt16(); break;
						case Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = node.GetUInt32(); break;
						case Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = node.GetUInt64(); break;
						case Type::Float32: *reinterpret_cast<float*>(variableAddress) = node.GetFloat32(); break;
						case Type::Float64: *reinterpret_cast<double*>(variableAddress) = node.GetFloat64(); break;
						}
					}
				}
			}
		}
	}
}
