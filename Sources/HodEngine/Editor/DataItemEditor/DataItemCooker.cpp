#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataItemEditor/DataItemCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/GameSystems/Resource/ResourceVariant.hpp"

#include <utility>

namespace hod::inline editor
{
	bool DataItemCooker::FillDataBlock(const Asset& asset, uint32_t /*platforms*/, uint8_t /*configs*/, uint32_t /*languages*/)
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		const AssetContainer::DataBlockInfo* settingsDataBlock = assetContainer.FindDataBlock("Settings");
		if (settingsDataBlock == nullptr)
		{
			OUTPUT_ERROR("DataItemCooker: missing 'Settings' data block");
			return false;
		}

		Vector<uint8_t> settingsData;
		settingsData.Resize(settingsDataBlock->_uncompressedSize);
		if (settingsDataBlock->_stream->Read(settingsData.Data(), settingsData.Size()) != settingsData.Size())
		{
			OUTPUT_ERROR("DataItemCooker: can't read 'Settings' data block");
			return false;
		}

		Stream& settingsStream = AddDataBlockStream("Settings", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		if (settingsStream.Write(settingsData.Data(), settingsData.Size()) != settingsData.Size())
		{
			OUTPUT_ERROR("DataItemCooker: unable to write 'Settings' data block");
			return false;
		}

		return true;
	}
}
