#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"

#include <type_traits>

namespace hod
{
    class ReflectionDescriptor;
    class ReflectionPropertyVariable;
    class ReflectionPropertyArray;
    class ReflectionPropertyObject;

    class HOD_CORE_API Serializer
    {
    public:

		static bool	Deserialize(const ReflectionDescriptor& reflectionDescriptor, void* instance, const Document::Node& documentNode, const std::function<bool(void*, const ReflectionDescriptor&, const Document::Node&)>& customDeserializationCallback = nullptr);
        static bool DeserializeWithPath(const std::string_view& path, const ReflectionDescriptor& reflectionDescriptor, void* instance, const Document::Node& documentNode);
        static bool	Serialize(const ReflectionDescriptor& reflectionDescriptor, const void* instance, Document::Node& documentNode, const std::function<bool(const void*, const ReflectionDescriptor&, Document::Node&)>& customSerializationCallback = nullptr);
        //static bool	SerializeDiff(const ReflectionDescriptor* reflectionDescriptor, const void* reference, const void* instance, Document::Node& documentNode);

        template<typename _InstanceType_>
        static bool Deserialize(_InstanceType_& instance, const Document::Node& documentNode, const std::function<bool(void*, const ReflectionDescriptor&, const Document::Node&)>& customDeserializationCallback = nullptr);

        template<typename _InstanceType_>
        static bool DeserializeWithPath(const std::string_view& path, _InstanceType_& instance, const Document::Node& documentNode);

        template<typename _InstanceType_>
        static bool Serialize(const _InstanceType_& instance, Document::Node& documentNode, const std::function<bool(const void*, const ReflectionDescriptor&, Document::Node&)>& customSerializationCallback = nullptr);

        //template<typename _InstanceType_>
        //static bool SerializeDiff(const _InstanceType_& reference, const _InstanceType_& instance, Document::Node& documentNode);


        static bool DeserializeVariable(const ReflectionPropertyVariable* property, void* instance, const Document::Node& documentNode, std::string_view overrideNodeName = std::string_view());
        static bool SerializeVariable(const ReflectionPropertyVariable* property, const void* instance, Document::Node& documentNode, std::string_view overrideNodeName = std::string_view());

        static bool DeserializeArray(const ReflectionPropertyArray* property, void* instance, const Document::Node& documentNode, std::string_view overrideNodeName = std::string_view());
        static bool SerializeArray(const ReflectionPropertyArray* property, const void* instance, Document::Node& documentNode, std::string_view overrideNodeName = std::string_view());

        static bool DeserializeObject(const ReflectionPropertyObject* property, void* instance, const Document::Node& documentNode, const std::function<bool(void*, const ReflectionDescriptor&, const Document::Node&)>& customDeserializationCallback, std::string_view overrideNodeName = std::string_view());
        static bool SerializeObject(const ReflectionPropertyObject* property, const void* instance, Document::Node& documentNode, const std::function<bool(const void*, const ReflectionDescriptor&, Document::Node&)>& customSerializationCallback, std::string_view overrideNodeName = std::string_view());
        //static bool SerializeDiffObject(const ReflectionPropertyObject* property, const void* reference, const void* instance, Document::Node& documentNode);
    };
}

#include "Serializer.inl"
