namespace hod
{
    template<typename _InstanceType_>
    bool Serializer::Serialize(const _InstanceType_& instance, Document::Node& documentNode)
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

	/*
	template<typename _InstanceType_>
    bool Serializer::SerializeDiff(const _InstanceType_& reference, const _InstanceType_& instance, Document::Node& documentNode)
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
    bool Serializer::Deserialize(_InstanceType_& instance, const Document::Node& documentNode)
	{
		if constexpr (std::is_pointer_v<_InstanceType_>)
		{
			return Serializer::Deserialize(instance->GetReflectionDescriptorV(), static_cast<void*>(instance), documentNode);
		}
		else
		{
			return Serializer::Deserialize(instance.GetReflectionDescriptorV(), static_cast<void*>(&instance), documentNode);
		}
	}

	template<typename _InstanceType_>
	bool Serializer::DeserializeWithPath(const std::string_view& path, _InstanceType_& instance, const Document::Node& documentNode)
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
