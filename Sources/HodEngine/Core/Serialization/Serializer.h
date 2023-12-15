#pragma once

#include "HodEngine/Core/Document/Document.h"

#include <type_traits>

namespace hod
{
    class ReflectionDescriptor;
    class ReflectionPropertyVariable;
    class ReflectionPropertyArray;
    class ReflectionPropertyObject;

    class Serializer
    {
    public:

        static bool	Serialize(const ReflectionDescriptor* reflectionDescriptor, const void* instance, Document::Node& documentNode);
		static bool	Deserialize(const ReflectionDescriptor* reflectionDescriptor, void* instance, const Document::Node& documentNode);

        template<typename _InstanceType_>
        static bool Serialize(const _InstanceType_& instance, Document::Node& documentNode);

        template<typename _InstanceType_>
        static bool Deserialize(_InstanceType_& instance, const Document::Node& documentNode);

    private:

        static bool SerializeVariable(const ReflectionPropertyVariable* property, const void* instance, Document::Node& documentNode);
        static bool DeserializeVariable(const ReflectionPropertyVariable* property, void* instance, const Document::Node& documentNode);

        static bool SerializeArray(const ReflectionPropertyArray* property, const void* instance, Document::Node& documentNode);
        static bool DeserializeArray(const ReflectionPropertyArray* property, void* instance, const Document::Node& documentNode);

        static bool SerializeObject(const ReflectionPropertyObject* property, const void* instance, Document::Node& documentNode);
        static bool DeserializeObject(const ReflectionPropertyObject* property, void* instance, const Document::Node& documentNode);
    };
}

#include "Serializer.inl"
