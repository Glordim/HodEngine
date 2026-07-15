#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataItemResource.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(DataItemResource, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
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

		if (Serializer::Deserialize(_dataItemContainer, settingsDocument.GetRootNode()) == false)
		{
			OUTPUT_ERROR("DataItemResource::Initialize: unable to deserialize 'Settings' data block");
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	DataStruct& DataItemResource::GetDataItem()
	{
		return *_dataItemContainer.GetData();
	}
}
