#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SerializedDataContainer.hpp"
#include "HodEngine/Game/SerializedData.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(SerializedDataContainer, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>([](const void* instance, DocumentNode& documentNode)
		{
			const SerializedDataContainer* serializedDataContainer = static_cast<const SerializedDataContainer*>(instance);

			documentNode.AddChild("Type").SetUInt64(serializedDataContainer->_data->GetReflectionDescriptorV().GetType());
			return Serializer::Serialize(static_cast<const SerializedData*>(serializedDataContainer->_data), documentNode.AddChild("Data"));
		},
		[](void* instance, const DocumentNode& documentNode)
		{
			SerializedDataContainer* serializedDataContainer = static_cast<SerializedDataContainer*>(instance);

			RttiType RttiType = documentNode.GetChild("Type")->GetUInt64();
			ReflectionDescriptor* reflectionDescriptor = SerializedDataFactory::GetInstance()->FindReflectionDescriptor(RttiType);
			DefaultAllocator::GetInstance().Delete(serializedDataContainer->_data);
			serializedDataContainer->_data = static_cast<SerializedData*>(reflectionDescriptor->CreateInstance());
			return Serializer::Deserialize(*static_cast<SerializedData*>(serializedDataContainer->_data), *documentNode.GetChild("Data"));
		});
	}

	SerializedDataContainer::SerializedDataContainer(SerializedData* serializedData)
	: _data(serializedData)
	{
	}

	SerializedDataContainer::~SerializedDataContainer()
	{
		DefaultAllocator::GetInstance().Delete(_data);
	}

	/// @brief
	/// @return
	SerializedData* SerializedDataContainer::GetData()
	{
		return _data;
	}
}
