#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/PrefabEditor/PrefabCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

namespace hod::inline editor
{
	bool PrefabCooker::FillDataBlock(const Asset& asset, uint32_t /*platforms*/, uint8_t /*configs*/, uint32_t /*languages*/)
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		const AssetContainer::DataBlockInfo* entitiesDataBlock = assetContainer.FindDataBlock("Entities");
		if (entitiesDataBlock == nullptr)
		{
			return false;
		}

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, *entitiesDataBlock->_stream) == false)
		{
			return false;
		}
		
		Stream& entitiesStream = AddDataBlockStream("Entities", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, entitiesStream) == false)
		{
			return false;
		}
		return true;
	}
}
