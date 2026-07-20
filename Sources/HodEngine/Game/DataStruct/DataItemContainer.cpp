#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataStruct/DataItemContainer.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(DataItemContainer, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>([](const void* instance, DocumentNode& documentNode)
		{
			const DataItemContainer* dataItemContainer = static_cast<const DataItemContainer*>(instance);

			documentNode.AddChild("Type").SetUInt64(dataItemContainer->_data->GetReflectionDescriptorV().GetType());
			return Serializer::Serialize(static_cast<const DataStruct*>(dataItemContainer->_data), documentNode.AddChild("Data"));
		},
		[](void* instance, const DocumentNode& documentNode)
		{
			DataItemContainer* dataItemContainer = static_cast<DataItemContainer*>(instance);

			RttiType RttiType = documentNode.GetChild("Type")->GetUInt64();
			ReflectionDescriptor* reflectionDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(RttiType);
			DefaultAllocator::GetInstance().Delete(dataItemContainer->_data);
			dataItemContainer->_data = static_cast<DataStruct*>(reflectionDescriptor->CreateInstance());
			return Serializer::Deserialize(*static_cast<DataStruct*>(dataItemContainer->_data), *documentNode.GetChild("Data"));
		});
	}

	DataItemContainer::DataItemContainer(DataStruct* dataItem)
	: _data(dataItem)
	{
	}

	DataItemContainer::~DataItemContainer()
	{
		DefaultAllocator::GetInstance().Delete(_data);
	}

	/// @brief
	/// @return
	DataStruct* DataItemContainer::GetData()
	{
		return _data;
	}
}
