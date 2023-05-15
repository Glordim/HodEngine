
namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				template<typename T>
				void Array::Serialize(const T& instance, Document::Node& node)
				{
					const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(&instance);
					const uint8_t* arrayAddress = instanceAddress + _offset;

					Document::Node& arrayNode = node.GetOrAddChild(_name);

						const std::vector<int8_t>* vec = reinterpret_cast<const std::vector<int8_t>*>(arrayAddress);
						auto valu = vec->data();
						const int8_t* val = vec->data();
						arrayNode.SetValues(std::span<const int8_t>(val, vec->size()));

					switch (_type)
					{
					case Variable::Type::Bool:
					{
						
					}
					break;
					case Variable::Type::Int8: arrayNode.SetValues(std::span<const int8_t>(*reinterpret_cast<const std::vector<int8_t>*>(arrayAddress))); break;
					case Variable::Type::Int16: arrayNode.SetValues(std::span<const int16_t>(*reinterpret_cast<const std::vector<int16_t>*>(arrayAddress))); break;
					case Variable::Type::Int32: arrayNode.SetValues(std::span<const int32_t>(*reinterpret_cast<const std::vector<int32_t>*>(arrayAddress))); break;
					case Variable::Type::Int64: arrayNode.SetValues(std::span<const int64_t>(*reinterpret_cast<const std::vector<int64_t>*>(arrayAddress))); break;
					case Variable::Type::UInt8: arrayNode.SetValues(std::span<const uint8_t>(*reinterpret_cast<const std::vector<uint8_t>*>(arrayAddress))); break;
					case Variable::Type::UInt16: arrayNode.SetValues(std::span<const uint16_t>(*reinterpret_cast<const std::vector<uint16_t>*>(arrayAddress))); break;
					case Variable::Type::UInt32: arrayNode.SetValues(std::span<const uint32_t>(*reinterpret_cast<const std::vector<uint32_t>*>(arrayAddress))); break;
					case Variable::Type::UInt64: arrayNode.SetValues(std::span<const uint64_t>(*reinterpret_cast<const std::vector<uint64_t>*>(arrayAddress))); break;
					case Variable::Type::Float32: arrayNode.SetValues(std::span<const float>(*reinterpret_cast<const std::vector<float>*>(arrayAddress))); break;
					case Variable::Type::Float64: arrayNode.SetValues(std::span<const double>(*reinterpret_cast<const std::vector<double>*>(arrayAddress))); break;
					case Variable::Type::String: arrayNode.SetValues(std::span<const std::string>(*reinterpret_cast<const std::vector<std::string>*>(arrayAddress))); break;
					}
				}

				template<typename T>
				void Array::Deserialize(T& instance, const Document::Node& node)
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(&instance);
					uint8_t* arrayAddress = instanceAddress + _offset;

					const Document::Node* arrayNode = node.GetChild(_name);
					if (arrayNode != nullptr)
					{
						// TODO reserve

						const Document::Node* varNode = arrayNode->GetFirstChild();
						while (varNode != nullptr)
						{
							switch (_type)
							{
							case Variable::Type::Bool: reinterpret_cast<std::vector<bool>*>(arrayAddress)->push_back(varNode->GetBool()); break;
							case Variable::Type::Int8: reinterpret_cast<std::vector<int8_t>*>(arrayAddress)->push_back(varNode->GetInt8()); break;
							case Variable::Type::Int16: reinterpret_cast<std::vector<int16_t>*>(arrayAddress)->push_back(varNode->GetInt16()); break;
							case Variable::Type::Int32: reinterpret_cast<std::vector<int32_t>*>(arrayAddress)->push_back(varNode->GetInt32()); break;
							case Variable::Type::Int64: reinterpret_cast<std::vector<int64_t>*>(arrayAddress)->push_back(varNode->GetInt64()); break;
							case Variable::Type::UInt8: reinterpret_cast<std::vector<uint8_t>*>(arrayAddress)->push_back(varNode->GetUInt8()); break;
							case Variable::Type::UInt16: reinterpret_cast<std::vector<uint16_t>*>(arrayAddress)->push_back(varNode->GetUInt16()); break;
							case Variable::Type::UInt32: reinterpret_cast<std::vector<uint32_t>*>(arrayAddress)->push_back(varNode->GetUInt32()); break;
							case Variable::Type::UInt64: reinterpret_cast<std::vector<uint64_t>*>(arrayAddress)->push_back(varNode->GetUInt64()); break;
							case Variable::Type::Float32: reinterpret_cast<std::vector<float>*>(arrayAddress)->push_back(varNode->GetFloat32()); break;
							case Variable::Type::Float64: reinterpret_cast<std::vector<double>*>(arrayAddress)->push_back(varNode->GetFloat64()); break;
							case Variable::Type::String: reinterpret_cast<std::vector<std::string>*>(arrayAddress)->push_back(varNode->GetString()); break;
							}
							
							varNode = varNode->GetNextSibling();
						}
					}
				}
			}
		}
	}
}
