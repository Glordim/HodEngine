
namespace hod
{
	namespace Reflection
	{
		namespace Property
		{
			/// @brief 
			/// @tparam _type_ 
			/// @param instance 
			/// @param index 
			/// @return 
			template<typename _type_>
			_type_ Array::GetValue(const void* instance, uint32_t index) const
			{
				const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
				const void* arrayAddress = reinterpret_cast<const void*>(instanceAddress + _offset);
				const void* variableAddress = _getElementAddressFunction((void*)arrayAddress, index);

				switch (_type)
				{
				case Variable::Type::Bool: return *reinterpret_cast<const bool*>(variableAddress); break;
				case Variable::Type::Int8: return *reinterpret_cast<const int8_t*>(variableAddress); break;
				case Variable::Type::Int16: return *reinterpret_cast<const int16_t*>(variableAddress); break;
				case Variable::Type::Int32: return *reinterpret_cast<const int32_t*>(variableAddress); break;
				case Variable::Type::Int64: return *reinterpret_cast<const int64_t*>(variableAddress); break;
				case Variable::Type::UInt8: return *reinterpret_cast<const uint8_t*>(variableAddress); break;
				case Variable::Type::UInt16: return *reinterpret_cast<const uint16_t*>(variableAddress); break;
				case Variable::Type::UInt32: return *reinterpret_cast<const uint32_t*>(variableAddress); break;
				case Variable::Type::UInt64: return *reinterpret_cast<const uint64_t*>(variableAddress); break;
				case Variable::Type::Float32: return *reinterpret_cast<const float*>(variableAddress); break;
				case Variable::Type::Float64: return *reinterpret_cast<const double*>(variableAddress); break;
				//case Variable::Type::String: return *reinterpret_cast<const std::string*>(variableAddress); break;
				}

				return _type_();
			}

			/// @brief 
			/// @tparam _type_ 
			/// @param instance 
			/// @param index 
			/// @param value 
			template<typename _type_>
			void Array::SetValue(void* instance, uint32_t index, _type_ value)
			{
				uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
				void* arrayAddress = reinterpret_cast<void*>(instanceAddress + _offset);
				void* variableAddress = _getElementAddressFunction(arrayAddress, index);

				switch (_type)
				{
				case Variable::Type::Bool: *reinterpret_cast<bool*>(variableAddress) = value; break;
				case Variable::Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = value; break;
				case Variable::Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = value; break;
				case Variable::Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = value; break;
				case Variable::Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = value; break;
				case Variable::Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = value; break;
				case Variable::Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = value; break;
				case Variable::Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = value; break;
				case Variable::Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = value; break;
				case Variable::Type::Float32: *reinterpret_cast<float*>(variableAddress) = value; break;
				case Variable::Type::Float64: *reinterpret_cast<double*>(variableAddress) = value; break;
				//case Variable::Type::String: *reinterpret_cast<std::string*>(variableAddress) = value; break;
				}
			}
		}
	}
}
