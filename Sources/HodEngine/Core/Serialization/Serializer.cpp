#include "Serializer.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitGetValueForSerialization.hpp"

namespace hod
{
	/// @brief 
	/// @param reflectionDescriptor 
	/// @param instance 
	/// @param documentNode 
	/// @return 
	bool Serializer::Serialize(const ReflectionDescriptor* reflectionDescriptor, const void* instance, Document::Node& documentNode)
	{
        const ReflectionDescriptor* parent = reflectionDescriptor->GetParent();
        if (parent != nullptr)
        {
            Serializer::Serialize(parent, instance, documentNode);
        }

		ReflectionTraitCustomSerialization* customSerialization = reflectionDescriptor->FindTrait<ReflectionTraitCustomSerialization>();
		if (customSerialization != nullptr)
		{
			customSerialization->Serialize(instance, documentNode);
		}
		else
		{
			for (const ReflectionProperty* property : reflectionDescriptor->GetProperties())
			{
                MetaType type = property->GetMetaType();
                switch (type)
                {
                case ReflectionPropertyVariable::GetMetaTypeStatic():
                {
                    Serializer::SerializeVariable(static_cast<const ReflectionPropertyVariable*>(property), instance, documentNode);
                }
                break;

                case ReflectionPropertyArray::GetMetaTypeStatic():
                {
                    Serializer::SerializeArray(static_cast<const ReflectionPropertyArray*>(property), instance, documentNode);
                }
                break;

                case ReflectionPropertyObject::GetMetaTypeStatic():
                {
                    Serializer::SerializeObject(static_cast<const ReflectionPropertyObject*>(property), instance, documentNode);
                }
                break;
                
                default:
                {
                    assert(false);
                    break;
                }
                }
            }
        }
		
		return true;
	}
	
	/// @brief 
	/// @param reflectionDescriptor 
	/// @param instance 
	/// @param documentNode 
	/// @return 
	bool Serializer::Deserialize(const ReflectionDescriptor* reflectionDescriptor, void* instance, const Document::Node& documentNode)
	{
        const ReflectionDescriptor* parent = reflectionDescriptor->GetParent();
        if (parent != nullptr)
        {
            Serializer::Deserialize(parent, instance, documentNode);
        }

		ReflectionTraitCustomSerialization* customSerialization = reflectionDescriptor->FindTrait<ReflectionTraitCustomSerialization>();
		if (customSerialization != nullptr)
		{
			customSerialization->Deserialize(instance, documentNode);
		}
		else
		{
			for (const ReflectionProperty* property : reflectionDescriptor->GetProperties())
			{
                MetaType type = property->GetMetaType();
                switch (type)
                {
                case ReflectionPropertyVariable::GetMetaTypeStatic():
                {
                    Serializer::DeserializeVariable(static_cast<const ReflectionPropertyVariable*>(property), instance, documentNode);
                }
                break;

                case ReflectionPropertyArray::GetMetaTypeStatic():
                {
                    Serializer::DeserializeArray(static_cast<const ReflectionPropertyArray*>(property), instance, documentNode);
                }
                break;

                case ReflectionPropertyObject::GetMetaTypeStatic():
                {
                    Serializer::DeserializeObject(static_cast<const ReflectionPropertyObject*>(property), instance, documentNode);
                }
                break;
                
                default:
                {
                    assert(false);
                    break;
                }
                }
            }
		}

		return true;
	}

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::SerializeVariable(const ReflectionPropertyVariable* property, const void* instance, Document::Node& documentNode)
    {
        const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
        const uint8_t* variableAddress = instanceAddress + property->GetOffset();

        Document::Node& varNode = documentNode.GetOrAddChild(property->GetName());
        
        switch (property->GetType())
        {
        case ReflectionPropertyVariable::Type::Bool: varNode.SetBool(*reinterpret_cast<const bool*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Int8: varNode.SetInt8(*reinterpret_cast<const int8_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Int16: varNode.SetInt16(*reinterpret_cast<const int16_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Int32: varNode.SetInt32(*reinterpret_cast<const int32_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Int64: varNode.SetInt64(*reinterpret_cast<const int64_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::UInt8: varNode.SetUInt8(*reinterpret_cast<const uint8_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::UInt16: varNode.SetUInt16(*reinterpret_cast<const uint16_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::UInt32: varNode.SetUInt32(*reinterpret_cast<const uint32_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::UInt64: varNode.SetUInt64(*reinterpret_cast<const uint64_t*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Float32: varNode.SetFloat32(*reinterpret_cast<const float*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Float64: varNode.SetFloat64(*reinterpret_cast<const double*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::String: varNode.SetString(*reinterpret_cast<const std::string*>(variableAddress)); break;
        case ReflectionPropertyVariable::Type::Object: varNode.SetString(*reinterpret_cast<const std::string*>(variableAddress)); break;

        default: assert(false); break;
        }

        return true;
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::DeserializeVariable(const ReflectionPropertyVariable* property, void* instance, const Document::Node& documentNode)
    {
        uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
        uint8_t* variableAddress = instanceAddress + property->GetOffset();

        const Document::Node* varNode = documentNode.GetChild(property->GetName());
        if (varNode != nullptr)
        {
            switch (property->GetType())
            {
            case ReflectionPropertyVariable::Type::Bool: *reinterpret_cast<bool*>(variableAddress) = varNode->GetBool(); break;
            case ReflectionPropertyVariable::Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = varNode->GetInt8(); break;
            case ReflectionPropertyVariable::Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = varNode->GetInt16(); break;
            case ReflectionPropertyVariable::Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = varNode->GetInt32(); break;
            case ReflectionPropertyVariable::Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = varNode->GetInt64(); break;
            case ReflectionPropertyVariable::Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = varNode->GetUInt8(); break;
            case ReflectionPropertyVariable::Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = varNode->GetUInt16(); break;
            case ReflectionPropertyVariable::Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = varNode->GetUInt32(); break;
            case ReflectionPropertyVariable::Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = varNode->GetUInt64(); break;
            case ReflectionPropertyVariable::Type::Float32: *reinterpret_cast<float*>(variableAddress) = varNode->GetFloat32(); break;
            case ReflectionPropertyVariable::Type::Float64: *reinterpret_cast<double*>(variableAddress) = varNode->GetFloat64(); break;
            case ReflectionPropertyVariable::Type::String: *reinterpret_cast<std::string*>(variableAddress) = varNode->GetString(); break;

            default: assert(false); break;
            }
        }

        return true;
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::SerializeArray(const ReflectionPropertyArray* property, const void* instance, Document::Node& documentNode)
    {
        const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
        const uint8_t* arrayAddress = instanceAddress + property->GetOffset();

        Document::Node& arrayNode = documentNode.GetOrAddChild(property->GetName());

        switch (property->GetType())
        {
        //case ReflectionPropertyVariable::Type::Bool: arrayNode.SetValues(std::span<const bool>(*reinterpret_cast<const std::vector<bool>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int8: arrayNode.SetValues(std::span<const int8_t>(*reinterpret_cast<const std::vector<int8_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int16: arrayNode.SetValues(std::span<const int16_t>(*reinterpret_cast<const std::vector<int16_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int32: arrayNode.SetValues(std::span<const int32_t>(*reinterpret_cast<const std::vector<int32_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int64: arrayNode.SetValues(std::span<const int64_t>(*reinterpret_cast<const std::vector<int64_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt8: arrayNode.SetValues(std::span<const uint8_t>(*reinterpret_cast<const std::vector<uint8_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt16: arrayNode.SetValues(std::span<const uint16_t>(*reinterpret_cast<const std::vector<uint16_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt32: arrayNode.SetValues(std::span<const uint32_t>(*reinterpret_cast<const std::vector<uint32_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt64: arrayNode.SetValues(std::span<const uint64_t>(*reinterpret_cast<const std::vector<uint64_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Float32: arrayNode.SetValues(std::span<const float>(*reinterpret_cast<const std::vector<float>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Float64: arrayNode.SetValues(std::span<const double>(*reinterpret_cast<const std::vector<double>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::String: arrayNode.SetValues(std::span<const std::string>(*reinterpret_cast<const std::vector<std::string>*>(arrayAddress))); break;
        //case ReflectionPropertyVariable::Type::Object: arrayNode.SetValues(std::span<const std::string>(*reinterpret_cast<const std::vector<std::string>*>(arrayAddress))); break; // TODO

        default: assert(false); break;
        }
        return true;
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::DeserializeArray(const ReflectionPropertyArray* property, void* instance, const Document::Node& documentNode)
    {
        uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
        uint8_t* arrayAddress = instanceAddress + property->GetOffset();

        const Document::Node* arrayNode = documentNode.GetChild(property->GetName());
        if (arrayNode != nullptr)
        {
            // TODO reserve

            const Document::Node* varNode = arrayNode->GetFirstChild();
            while (varNode != nullptr)
            {
                switch (property->GetType())
                {
                case ReflectionPropertyVariable::Type::Bool: reinterpret_cast<std::vector<bool>*>(arrayAddress)->push_back(varNode->GetBool()); break;
                case ReflectionPropertyVariable::Type::Int8: reinterpret_cast<std::vector<int8_t>*>(arrayAddress)->push_back(varNode->GetInt8()); break;
                case ReflectionPropertyVariable::Type::Int16: reinterpret_cast<std::vector<int16_t>*>(arrayAddress)->push_back(varNode->GetInt16()); break;
                case ReflectionPropertyVariable::Type::Int32: reinterpret_cast<std::vector<int32_t>*>(arrayAddress)->push_back(varNode->GetInt32()); break;
                case ReflectionPropertyVariable::Type::Int64: reinterpret_cast<std::vector<int64_t>*>(arrayAddress)->push_back(varNode->GetInt64()); break;
                case ReflectionPropertyVariable::Type::UInt8: reinterpret_cast<std::vector<uint8_t>*>(arrayAddress)->push_back(varNode->GetUInt8()); break;
                case ReflectionPropertyVariable::Type::UInt16: reinterpret_cast<std::vector<uint16_t>*>(arrayAddress)->push_back(varNode->GetUInt16()); break;
                case ReflectionPropertyVariable::Type::UInt32: reinterpret_cast<std::vector<uint32_t>*>(arrayAddress)->push_back(varNode->GetUInt32()); break;
                case ReflectionPropertyVariable::Type::UInt64: reinterpret_cast<std::vector<uint64_t>*>(arrayAddress)->push_back(varNode->GetUInt64()); break;
                case ReflectionPropertyVariable::Type::Float32: reinterpret_cast<std::vector<float>*>(arrayAddress)->push_back(varNode->GetFloat32()); break;
                case ReflectionPropertyVariable::Type::Float64: reinterpret_cast<std::vector<double>*>(arrayAddress)->push_back(varNode->GetFloat64()); break;
                case ReflectionPropertyVariable::Type::String: reinterpret_cast<std::vector<std::string>*>(arrayAddress)->push_back(varNode->GetString()); break;
                //case ReflectionPropertyVariable::Type::Object: reinterpret_cast<std::vector<std::string>*>(arrayAddress)->push_back(varNode->GetString()); break; // TODO

                default: assert(false); break;
                }
                
                varNode = varNode->GetNextSibling();
            }
        }
        return true;
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::SerializeObject(const ReflectionPropertyObject* property, const void* instance, Document::Node& documentNode)
    {
        Document::Node& objectNode = documentNode.GetOrAddChild(property->GetName());

        const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
        const uint8_t* objectAddress = instanceAddress + property->GetOffset();
        const void* objectInstance = reinterpret_cast<const void*>(objectAddress);

        ReflectionTraitGetValueForSerialization* traitGetValueForSerialization = property->FindTrait<ReflectionTraitGetValueForSerialization>();
        if (traitGetValueForSerialization != nullptr)
        {
            uint8_t* buffer = new uint8_t[traitGetValueForSerialization->GetSize()]; // todo pmr stack alloc
            traitGetValueForSerialization->GetValueForSerialization(objectInstance, buffer);
            bool result = Serializer::Serialize(property->GetReflectionDescriptor(), buffer, objectNode);
            delete[] buffer;
            return result;
        }
        else
        {            
            return Serializer::Serialize(property->GetReflectionDescriptor(), objectInstance, objectNode);
        }
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::DeserializeObject(const ReflectionPropertyObject* property, void* instance, const Document::Node& documentNode)
    {
        uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
        uint8_t* objectAddress = instanceAddress + property->GetOffset();

        void* objectInstance = reinterpret_cast<void*>(objectAddress);

        const Document::Node* objectNode = documentNode.GetChild(property->GetName());
        
        if (objectNode != nullptr)
        {
            return Serializer::Deserialize(property->GetReflectionDescriptor(), objectInstance, *objectNode);
        }
        return true;
    }
}
