#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"

namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				Variable::Variable(Type type, uint32_t offset, const char* name)
					: ReflectionProperty("Variable")
					, _type(type)
					, _offset(offset)
					, _name(name)
				{

				}

				void Variable::Serialize(const void* instance, Document::Node& node)
				{
					const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
					const uint8_t* variableAddress = instanceAddress + _offset;

					Document::Node& varNode = node.GetOrAddChild(_name);
					
					switch (_type)
					{
					case Type::Bool: varNode.SetBool(*reinterpret_cast<const bool*>(variableAddress)); break;
					case Type::Int8: varNode.SetInt8(*reinterpret_cast<const int8_t*>(variableAddress)); break;
					case Type::Int16: varNode.SetInt16(*reinterpret_cast<const int16_t*>(variableAddress)); break;
					case Type::Int32: varNode.SetInt32(*reinterpret_cast<const int32_t*>(variableAddress)); break;
					case Type::Int64: varNode.SetInt64(*reinterpret_cast<const int64_t*>(variableAddress)); break;
					case Type::UInt8: varNode.SetUInt8(*reinterpret_cast<const uint8_t*>(variableAddress)); break;
					case Type::UInt16: varNode.SetUInt16(*reinterpret_cast<const uint16_t*>(variableAddress)); break;
					case Type::UInt32: varNode.SetUInt32(*reinterpret_cast<const uint32_t*>(variableAddress)); break;
					case Type::UInt64: varNode.SetUInt64(*reinterpret_cast<const uint64_t*>(variableAddress)); break;
					case Type::Float32: varNode.SetFloat32(*reinterpret_cast<const float*>(variableAddress)); break;
					case Type::Float64: varNode.SetFloat64(*reinterpret_cast<const double*>(variableAddress)); break;
					case Type::String: varNode.SetString(*reinterpret_cast<const std::string*>(variableAddress)); break;
					case Type::Object: varNode.SetString(*reinterpret_cast<const std::string*>(variableAddress)); break;
					}
				}

				void Variable::Deserialize(void* instance, const Document::Node& node)
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
					uint8_t* variableAddress = instanceAddress + _offset;

					const Document::Node* varNode = node.GetChild(_name);
					if (varNode != nullptr)
					{
						switch (_type)
						{
						case Type::Bool: *reinterpret_cast<bool*>(variableAddress) = varNode->GetBool(); break;
						case Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = varNode->GetInt8(); break;
						case Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = varNode->GetInt16(); break;
						case Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = varNode->GetInt32(); break;
						case Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = varNode->GetInt64(); break;
						case Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = varNode->GetUInt8(); break;
						case Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = varNode->GetUInt16(); break;
						case Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = varNode->GetUInt32(); break;
						case Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = varNode->GetUInt64(); break;
						case Type::Float32: *reinterpret_cast<float*>(variableAddress) = varNode->GetFloat32(); break;
						case Type::Float64: *reinterpret_cast<double*>(variableAddress) = varNode->GetFloat64(); break;
						case Type::String: *reinterpret_cast<std::string*>(variableAddress) = varNode->GetString(); break;
						}
					}
				}
			}
		}
	}
}
