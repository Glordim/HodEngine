#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataStruct/DataItemResource.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(DataItemResource) {}

	DataItemResource::~DataItemResource()
	{
		if (_data != nullptr)
		{
			_data->GetReflectionDescriptorV().DeleteInstance(_data);
		}
	}

	/// @brief
	/// @return
	bool DataItemResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* settingsDataBlock = resourceContainer.FindDataBlock("Settings");
		if (settingsDataBlock == nullptr)
		{
			OUTPUT_ERROR("DataItemResource::Initialize: missing 'Settings' data block");
			return false;
		}

		Document           settingsDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == false)
		{
			OUTPUT_ERROR("DataItemResource::Initialize: unable to read 'Settings' data block");
			return false;
		}

		_data = Serializer::DeserializeWithType<DataStruct>(settingsDocument.GetRootNode(),
		                                                    [](RttiType type) { return DataStructFactory::GetInstance()->FindReflectionDescriptor(type); });
		if (_data == nullptr)
		{
			OUTPUT_ERROR("DataItemResource::Initialize: unable to read its data (data struct {}, unknown when its module isn't loaded)",
			             Serializer::ReadType(settingsDocument.GetRootNode()));
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	const DataStruct* DataItemResource::GetDataItem() const
	{
		return _data;
	}
}
