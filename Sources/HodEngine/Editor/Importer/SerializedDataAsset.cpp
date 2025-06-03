#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"
#include "HodEngine/Game/SerializedData.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(SerializedDataAsset, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>([](const void* instance, Document::Node& documentNode)
		{
			const SerializedDataAsset* serializedDataAsset = static_cast<const SerializedDataAsset*>(instance);

			documentNode.AddChild("Type").SetUInt64(serializedDataAsset->_data->GetReflectionDescriptorV().GetMetaType());
			Serializer::Serialize(static_cast<const game::SerializedData*>(serializedDataAsset->_data), documentNode.AddChild("Data"));
		},
		[](void* instance, const Document::Node& documentNode)
		{
			SerializedDataAsset* serializedDataAsset = static_cast<SerializedDataAsset*>(instance);

			MetaType metaType = documentNode.GetChild("Type")->GetUInt64();
			ReflectionDescriptor* reflectionDescriptor = game::SerializedDataFactory::GetInstance()->FindReflectionDescriptor(metaType);
			delete serializedDataAsset->_data;
			serializedDataAsset->_data = static_cast<game::SerializedData*>(reflectionDescriptor->CreateInstance());
			Serializer::Deserialize(*static_cast<game::SerializedData*>(serializedDataAsset->_data), *documentNode.GetChild("Data"));
		});
	}

	SerializedDataAsset::SerializedDataAsset(game::SerializedData* serializedData)
	: _data(serializedData)
	{
		
	}

	SerializedDataAsset::~SerializedDataAsset()
	{
		delete _data;
	}

	/// @brief 
	/// @return 
	game::SerializedData* SerializedDataAsset::GetData()
	{
		return _data;
	}
}
