#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitSerializedByCallback.hpp"

#include <cstring>

namespace hod
{
	/// @brief 
	/// @param reflectionDescriptor 
	/// @param instance 
	/// @param documentNode 
	/// @return 
	bool Serializer::Serialize(const ReflectionDescriptor& reflectionDescriptor, const void* instance, Document::Node& documentNode, const std::function<bool(const void*, const ReflectionDescriptor&, Document::Node&)>& customSerializationCallback)
	{
		const ReflectionDescriptor* parent = reflectionDescriptor.GetParent();
		if (parent != nullptr)
		{
			Serializer::Serialize(*parent, instance, documentNode);
		}

		if (reflectionDescriptor.FindTrait<ReflectionTraitSerializedByCallback>() != nullptr)
		{
			assert(customSerializationCallback);
			customSerializationCallback(instance, reflectionDescriptor, documentNode);
		}
		else
		{
			ReflectionTraitCustomSerialization* customSerialization = reflectionDescriptor.FindTrait<ReflectionTraitCustomSerialization>();
			if (customSerialization != nullptr)
			{
				customSerialization->Serialize(instance, documentNode);
			}
			else
			{
				for (const ReflectionProperty* property : reflectionDescriptor.GetProperties())
				{
					ReflectionTraitNoSerialization* noSerialization = property->FindTrait<ReflectionTraitNoSerialization>();
					if (noSerialization != nullptr)
					{
						continue;
					}
					
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
						Serializer::SerializeObject(static_cast<const ReflectionPropertyObject*>(property), instance, documentNode, customSerializationCallback);
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
		}
		
		return true;
	}

	/*
	/// @brief 
	/// @param reflectionDescriptor 
	/// @param reference 
	/// @param instance 
	/// @param documentNode 
	/// @return 
	bool Serializer::SerializeDiff(const ReflectionDescriptor* reflectionDescriptor, const void* reference, const void* instance, Document::Node& documentNode)
	{
        const ReflectionDescriptor* parent = reflectionDescriptor->GetParent();
        if (parent != nullptr)
        {
            Serializer::SerializeDiff(parent, reference, instance, documentNode);
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
                    //Serializer::SerializeDiffVariable(static_cast<const ReflectionPropertyVariable*>(property), reference, instance, documentNode);
                }
                break;

                case ReflectionPropertyArray::GetMetaTypeStatic():
                {
                    Serializer::SerializeArray(static_cast<const ReflectionPropertyArray*>(property), instance, documentNode);
                }
                break;

                case ReflectionPropertyObject::GetMetaTypeStatic():
                {
                    Serializer::SerializeDiffObject(static_cast<const ReflectionPropertyObject*>(property), reference, instance, documentNode);
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
	*/

	/// @brief 
	/// @param reflectionDescriptor 
	/// @param instance 
	/// @param documentNode 
	/// @return 
	bool Serializer::Deserialize(const ReflectionDescriptor& reflectionDescriptor, void* instance, const Document::Node& documentNode, const std::function<bool(void*, const ReflectionDescriptor&, const Document::Node&)>& customDeserializationCallback)
	{
		const ReflectionDescriptor* parent = reflectionDescriptor.GetParent();
		if (parent != nullptr)
		{
			if (Serializer::Deserialize(*parent, instance, documentNode) == false)
			{
				return false;
			}
		}

		if (reflectionDescriptor.FindTrait<ReflectionTraitSerializedByCallback>() != nullptr)
		{
			assert(customDeserializationCallback);
			if (customDeserializationCallback(instance, reflectionDescriptor, documentNode) == false)
			{
				return false;
			}
		}
		else
		{
			ReflectionTraitCustomSerialization* customSerialization = reflectionDescriptor.FindTrait<ReflectionTraitCustomSerialization>();
			if (customSerialization != nullptr)
			{
				if (customSerialization->Deserialize(instance, documentNode) == false)
				{
					return false;
				}
			}
			else
			{
				for (const ReflectionProperty* property : reflectionDescriptor.GetProperties())
				{
					ReflectionTraitNoSerialization* noSerialization = property->FindTrait<ReflectionTraitNoSerialization>();
					if (noSerialization != nullptr)
					{
						continue;
					}

					MetaType type = property->GetMetaType();
					switch (type)
					{
					case ReflectionPropertyVariable::GetMetaTypeStatic():
					{
						if (Serializer::DeserializeVariable(static_cast<const ReflectionPropertyVariable*>(property), instance, documentNode) == false)
						{
							return false;
						}
					}
					break;

					case ReflectionPropertyArray::GetMetaTypeStatic():
					{
						if (Serializer::DeserializeArray(static_cast<const ReflectionPropertyArray*>(property), instance, documentNode) == false)
						{
							return false;
						}
					}
					break;

					case ReflectionPropertyObject::GetMetaTypeStatic():
					{
						if (Serializer::DeserializeObject(static_cast<const ReflectionPropertyObject*>(property), instance, documentNode, customDeserializationCallback) == false)
						{
							return false;
						}
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
		}

		return true;
	}

    /// @brief 
    /// @param path 
    /// @param reflectionDescriptor 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::DeserializeWithPath(const std::string_view& path, const ReflectionDescriptor& reflectionDescriptor, void* instance, const Document::Node& documentNode)
    {
		const ReflectionDescriptor* finalReflectionDescriptor = &reflectionDescriptor;

        String propertyPath(path.data(), path.size());
        char* next = std::strpbrk((char*)propertyPath.c_str(), ".");
        while (next != nullptr)
        {
            std::string_view elementName(propertyPath.c_str(), next);
            ReflectionPropertyObject* property = reflectionDescriptor.FindProperty<ReflectionPropertyObject>(elementName);
            if (property != nullptr)
            {
                instance = property->GetInstance(instance);
                finalReflectionDescriptor = property->GetReflectionDescriptor();
            }
            propertyPath = next + 1; // skip '/'
            next = std::strpbrk((char*)propertyPath.c_str(), ".");
        }

        next = std::strpbrk((char*)propertyPath.c_str(), "[");
        if (next != nullptr) // array
        {
            /*
            ReflectionPropertyArray* arrayProperty = reflectionDescriptor->FindProperty<ReflectionPropertyVariable>(propertyPath.c_str());
            Serializer::DeserializeArray(variableProperty, instance, documentNode, path);
            */
            return false;
        }
        else // variable
        {
            ReflectionPropertyVariable* variableProperty = finalReflectionDescriptor->FindProperty<ReflectionPropertyVariable>(propertyPath.c_str());
            return Serializer::DeserializeVariable(variableProperty, instance, documentNode, path);
        }
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::SerializeVariable(const ReflectionPropertyVariable* property, const void* instance, Document::Node& documentNode, std::string_view overrideNodeName)
    {
        const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
        const uint8_t* variableAddress = instanceAddress + property->GetOffset();

        if (overrideNodeName.empty())
        {
            overrideNodeName = property->GetName();
        }
        Document::Node& varNode = documentNode.GetOrAddChild(overrideNodeName);
        
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
    bool Serializer::DeserializeVariable(const ReflectionPropertyVariable* property, void* instance, const Document::Node& documentNode, std::string_view overrideNodeName)
    {
        uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
        uint8_t* variableAddress = instanceAddress + property->GetOffset();

        if (overrideNodeName.empty())
        {
            overrideNodeName = property->GetName();
        }
        const Document::Node* varNode = documentNode.GetChild(overrideNodeName);
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
    bool Serializer::SerializeArray(const ReflectionPropertyArray* property, const void* instance, Document::Node& documentNode, std::string_view overrideNodeName)
    {
        const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
        const uint8_t* arrayAddress = instanceAddress + property->GetOffset();

        if (overrideNodeName.empty())
        {
            overrideNodeName = property->GetName();
        }
        Document::Node& arrayNode = documentNode.GetOrAddChild(overrideNodeName);

        switch (property->GetType())
        {
        //case ReflectionPropertyVariable::Type::Bool: arrayNode.SetValues(std::span<const bool>(*reinterpret_cast<const Vector<bool>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int8: arrayNode.SetValues(std::span<const int8_t>(*reinterpret_cast<const Vector<int8_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int16: arrayNode.SetValues(std::span<const int16_t>(*reinterpret_cast<const Vector<int16_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int32: arrayNode.SetValues(std::span<const int32_t>(*reinterpret_cast<const Vector<int32_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Int64: arrayNode.SetValues(std::span<const int64_t>(*reinterpret_cast<const Vector<int64_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt8: arrayNode.SetValues(std::span<const uint8_t>(*reinterpret_cast<const Vector<uint8_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt16: arrayNode.SetValues(std::span<const uint16_t>(*reinterpret_cast<const Vector<uint16_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt32: arrayNode.SetValues(std::span<const uint32_t>(*reinterpret_cast<const Vector<uint32_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::UInt64: arrayNode.SetValues(std::span<const uint64_t>(*reinterpret_cast<const Vector<uint64_t>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Float32: arrayNode.SetValues(std::span<const float>(*reinterpret_cast<const Vector<float>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Float64: arrayNode.SetValues(std::span<const double>(*reinterpret_cast<const Vector<double>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::String: arrayNode.SetValues(std::span<const std::string>(*reinterpret_cast<const Vector<std::string>*>(arrayAddress))); break;
        case ReflectionPropertyVariable::Type::Object:
        {
            uint32_t elementCount = property->GetElementCount(instance);
            if (elementCount == 0)
            {
                arrayNode.SetTye(Document::Node::Type::Array);
            }
            else
            {
                for (uint32_t elementIndex = 0; elementIndex < elementCount; ++elementIndex)
                {
                    void* element = property->GetValue<void*>(instance, elementIndex);
                    if (Serializer::Serialize(*property->GetElementReflectionDescriptor(), element, arrayNode.AddChild("")) == false)
                    {
                        return false;
                    }
                }
            }
        }
        break;

        default: assert(false); break;
        }
        return true;
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::DeserializeArray(const ReflectionPropertyArray* property, void* instance, const Document::Node& documentNode, std::string_view overrideNodeName)
    {
        uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
        uint8_t* arrayAddress = instanceAddress + property->GetOffset();

        if (overrideNodeName.empty())
        {
            overrideNodeName = property->GetName();
        }
        const Document::Node* arrayNode = documentNode.GetChild(overrideNodeName);
        if (arrayNode != nullptr)
        {
            // TODO reserve
            property->Clear(instance);

            const Document::Node* varNode = arrayNode->GetFirstChild();
            while (varNode != nullptr)
            {
                switch (property->GetType())
                {
                case ReflectionPropertyVariable::Type::Bool: reinterpret_cast<Vector<bool>*>(arrayAddress)->push_back(varNode->GetBool()); break;
                case ReflectionPropertyVariable::Type::Int8: reinterpret_cast<Vector<int8_t>*>(arrayAddress)->push_back(varNode->GetInt8()); break;
                case ReflectionPropertyVariable::Type::Int16: reinterpret_cast<Vector<int16_t>*>(arrayAddress)->push_back(varNode->GetInt16()); break;
                case ReflectionPropertyVariable::Type::Int32: reinterpret_cast<Vector<int32_t>*>(arrayAddress)->push_back(varNode->GetInt32()); break;
                case ReflectionPropertyVariable::Type::Int64: reinterpret_cast<Vector<int64_t>*>(arrayAddress)->push_back(varNode->GetInt64()); break;
                case ReflectionPropertyVariable::Type::UInt8: reinterpret_cast<Vector<uint8_t>*>(arrayAddress)->push_back(varNode->GetUInt8()); break;
                case ReflectionPropertyVariable::Type::UInt16: reinterpret_cast<Vector<uint16_t>*>(arrayAddress)->push_back(varNode->GetUInt16()); break;
                case ReflectionPropertyVariable::Type::UInt32: reinterpret_cast<Vector<uint32_t>*>(arrayAddress)->push_back(varNode->GetUInt32()); break;
                case ReflectionPropertyVariable::Type::UInt64: reinterpret_cast<Vector<uint64_t>*>(arrayAddress)->push_back(varNode->GetUInt64()); break;
                case ReflectionPropertyVariable::Type::Float32: reinterpret_cast<Vector<float>*>(arrayAddress)->push_back(varNode->GetFloat32()); break;
                case ReflectionPropertyVariable::Type::Float64: reinterpret_cast<Vector<double>*>(arrayAddress)->push_back(varNode->GetFloat64()); break;
                case ReflectionPropertyVariable::Type::String: reinterpret_cast<Vector<std::string>*>(arrayAddress)->push_back(varNode->GetString()); break;
                case ReflectionPropertyVariable::Type::Object:
                {
                    property->InsertElement(instance, property->GetElementCount(instance));
                    void* element = property->GetValue<void*>(instance, property->GetElementCount(instance) - 1);
                    if (Serializer::Deserialize(*property->GetElementReflectionDescriptor(), element, *varNode) == false)
                    {
                        return false;
                    }
                }
                break;

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
    bool Serializer::SerializeObject(const ReflectionPropertyObject* property, const void* instance, Document::Node& documentNode, const std::function<bool(const void*, const ReflectionDescriptor&, Document::Node&)>& customSerializationCallback, std::string_view overrideNodeName)
    {
        if (overrideNodeName.empty())
        {
            overrideNodeName = property->GetName();
        }
        Document::Node& objectNode = documentNode.GetOrAddChild(overrideNodeName);

        const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
        const uint8_t* objectAddress = instanceAddress + property->GetOffset();
        const void* objectInstance = reinterpret_cast<const void*>(objectAddress);

        return Serializer::Serialize(*property->GetReflectionDescriptor(), objectInstance, objectNode, customSerializationCallback);
    }

    /// @brief 
    /// @param property 
    /// @param instance 
    /// @param documentNode 
    /// @return 
    bool Serializer::DeserializeObject(const ReflectionPropertyObject* property, void* instance, const Document::Node& documentNode, const std::function<bool(void*, const ReflectionDescriptor&, const Document::Node&)>& customDeserializationCallback, std::string_view overrideNodeName)
    {
        if (overrideNodeName.empty())
        {
            overrideNodeName = property->GetName();
        }
        const Document::Node* objectNode = documentNode.GetChild(overrideNodeName);
        
        if (objectNode != nullptr)
        {
            /*
            if (property->HasSetMethod())
            {
                void* newInstance = property->GetReflectionDescriptor()->CreateInstance();
                bool result = Serializer::Deserialize(property->GetReflectionDescriptor(), newInstance, *objectNode);
                if (result == false)
                {
                    property->GetReflectionDescriptor()->DeleteInstance(newInstance);
                    return false;
                }

                property->SetValue(instance, newInstance);

                property->GetReflectionDescriptor()->DeleteInstance(newInstance);
                return true;
            }
            else
            */
            {
                void* objectInstance = property->GetInstance(instance);
                return Serializer::Deserialize(*property->GetReflectionDescriptor(), objectInstance, *objectNode, customDeserializationCallback);
            }
        }
        return true;
    }
}
