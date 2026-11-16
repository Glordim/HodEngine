#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Reflection/Rtti.hpp"

#include <functional>
#include <type_traits>

namespace hod::inline core
{
	class ReflectionDescriptor;
	class ReflectionPropertyVariable;
	class ReflectionPropertyArray;
	class ReflectionPropertyObject;

	class HOD_CORE_API Serializer
	{
	public:
		static bool Deserialize(const ReflectionDescriptor& reflectionDescriptor, void* instance, const DocumentNode& documentNode,
		                        const std::function<bool(void*, const ReflectionDescriptor&, const DocumentNode&)>& customDeserializationCallback = nullptr);
		static bool DeserializeWithPath(const std::string_view& path, const ReflectionDescriptor& reflectionDescriptor, void* instance, const DocumentNode& documentNode);
		static bool Serialize(const ReflectionDescriptor& reflectionDescriptor, const void* instance, DocumentNode& documentNode,
		                      const std::function<bool(const void*, const ReflectionDescriptor&, DocumentNode&)>& customSerializationCallback = nullptr);
		// static bool	SerializeDiff(const ReflectionDescriptor* reflectionDescriptor, const void* reference, const void* instance, DocumentNode& documentNode);

		template<typename _InstanceType_>
		static bool Deserialize(_InstanceType_& instance, const DocumentNode& documentNode,
		                        const std::function<bool(void*, const ReflectionDescriptor&, const DocumentNode&)>& customDeserializationCallback = nullptr);

		template<typename _InstanceType_>
		static bool DeserializeWithPath(const std::string_view& path, _InstanceType_& instance, const DocumentNode& documentNode);

		template<typename _InstanceType_>
		static bool Serialize(const _InstanceType_& instance, DocumentNode& documentNode,
		                      const std::function<bool(const void*, const ReflectionDescriptor&, DocumentNode&)>& customSerializationCallback = nullptr);

		// template<typename _InstanceType_>
		// static bool SerializeDiff(const _InstanceType_& reference, const _InstanceType_& instance, DocumentNode& documentNode);

		/// @brief An instance whose type is known at runtime only (polymorphic): its "Type" (RttiType) then its properties, in the same node
		static bool SerializeWithType(const ReflectionDescriptor& reflectionDescriptor, const void* instance, DocumentNode& documentNode);
		/// @brief Of its concrete type (GetReflectionDescriptorV)
		template<typename _InstanceType_>
		static bool SerializeWithType(const _InstanceType_& instance, DocumentNode& documentNode);

		/// @return The "Type" written by SerializeWithType, 0 without one
		static RttiType ReadType(const DocumentNode& documentNode);

		/// @brief Creates the instance written by SerializeWithType and fills it
		/// @param findDescriptor The types allowed there (the caller's registry, a factory): null for an unknown type
		/// @param expectedBase Its type derives from it
		/// @return The instance (to delete through its descriptor, DeleteInstance), null when it can't be read. Null without an error
		/// for an unknown type (a disabled module's: the caller decides, error or data kept as is, ReadType tells it), with one otherwise
		static void* DeserializeWithType(const DocumentNode& documentNode, const std::function<const ReflectionDescriptor*(RttiType)>& findDescriptor,
		                                 const ReflectionDescriptor& expectedBase);
		template<typename _InstanceType_>
		static _InstanceType_* DeserializeWithType(const DocumentNode& documentNode, const std::function<const ReflectionDescriptor*(RttiType)>& findDescriptor);

		static bool DeserializeVariable(const ReflectionPropertyVariable* property, void* instance, const DocumentNode& documentNode,
		                                std::string_view overrideNodeName = std::string_view());
		static bool SerializeVariable(const ReflectionPropertyVariable* property, const void* instance, DocumentNode& documentNode,
		                              std::string_view overrideNodeName = std::string_view());

		static bool DeserializeArray(const ReflectionPropertyArray* property, void* instance, const DocumentNode& documentNode,
		                             std::string_view overrideNodeName = std::string_view());
		static bool SerializeArray(const ReflectionPropertyArray* property, const void* instance, DocumentNode& documentNode,
		                           std::string_view overrideNodeName = std::string_view());

		static bool DeserializeObject(const ReflectionPropertyObject* property, void* instance, const DocumentNode& documentNode,
		                              const std::function<bool(void*, const ReflectionDescriptor&, const DocumentNode&)>& customDeserializationCallback,
		                              std::string_view                                                                      overrideNodeName = std::string_view());
		static bool SerializeObject(const ReflectionPropertyObject* property, const void* instance, DocumentNode& documentNode,
		                            const std::function<bool(const void*, const ReflectionDescriptor&, DocumentNode&)>& customSerializationCallback,
		                            std::string_view                                                                      overrideNodeName = std::string_view());
		// static bool SerializeDiffObject(const ReflectionPropertyObject* property, const void* reference, const void* instance, DocumentNode& documentNode);
	};

	template<typename _InstanceType_>
	bool Serializer::Serialize(const _InstanceType_& instance, DocumentNode& documentNode,
	                           const std::function<bool(const void*, const ReflectionDescriptor&, DocumentNode&)>& customSerializationCallback)
	{
		if constexpr (std::is_pointer_v<_InstanceType_>)
		{
			return Serializer::Serialize(instance->GetReflectionDescriptorV(), static_cast<const void*>(instance), documentNode, customSerializationCallback);
		}
		else
		{
			return Serializer::Serialize(instance.GetReflectionDescriptorV(), static_cast<const void*>(&instance), documentNode, customSerializationCallback);
		}
	}

	/*
	template<typename _InstanceType_>
	bool Serializer::SerializeDiff(const _InstanceType_& reference, const _InstanceType_& instance, DocumentNode& documentNode)
	{
	    if constexpr (std::is_pointer_v<_InstanceType_>)
	    {
	        return Serializer::Serialize(instance->GetReflectionDescriptorV(), static_cast<const void*>(instance), documentNode);
	    }
	    else
	    {
	        return Serializer::Serialize(instance.GetReflectionDescriptorV(), static_cast<const void*>(&instance), documentNode);
	    }
	}
	*/

	template<typename _InstanceType_>
	bool Serializer::SerializeWithType(const _InstanceType_& instance, DocumentNode& documentNode)
	{
		return Serializer::SerializeWithType(instance.GetReflectionDescriptorV(), static_cast<const void*>(&instance), documentNode);
	}

	template<typename _InstanceType_>
	_InstanceType_* Serializer::DeserializeWithType(const DocumentNode& documentNode, const std::function<const ReflectionDescriptor*(RttiType)>& findDescriptor)
	{
		return static_cast<_InstanceType_*>(Serializer::DeserializeWithType(documentNode, findDescriptor, _InstanceType_::GetReflectionDescriptor()));
	}

	template<typename _InstanceType_>
	bool Serializer::Deserialize(_InstanceType_& instance, const DocumentNode& documentNode,
	                             const std::function<bool(void*, const ReflectionDescriptor&, const DocumentNode&)>& customDeserializationCallback)
	{
		if constexpr (std::is_pointer_v<_InstanceType_>)
		{
			return Serializer::Deserialize(instance->GetReflectionDescriptorV(), static_cast<void*>(instance), documentNode, customDeserializationCallback);
		}
		else
		{
			return Serializer::Deserialize(instance.GetReflectionDescriptorV(), static_cast<void*>(&instance), documentNode, customDeserializationCallback);
		}
	}

	template<typename _InstanceType_>
	bool Serializer::DeserializeWithPath(const std::string_view& path, _InstanceType_& instance, const DocumentNode& documentNode)
	{
		if constexpr (std::is_pointer_v<_InstanceType_>)
		{
			return Serializer::DeserializeWithPath(path, instance->GetReflectionDescriptorV(), static_cast<void*>(instance), documentNode);
		}
		else
		{
			return Serializer::DeserializeWithPath(path, instance.GetReflectionDescriptorV(), static_cast<void*>(&instance), documentNode);
		}
	}
}
