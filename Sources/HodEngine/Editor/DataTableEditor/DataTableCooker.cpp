#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Game/DataTableContainer.hpp"
#include "HodEngine/Game/DataStruct.hpp"
#include <HodEngine/GameSystems/Resource/ResourceVariant.hpp>

#include <utility>

namespace hod::inline editor
{
	bool DataTableCooker::FillDataBlock(const Asset& asset, uint32_t /*platforms*/, uint8_t /*configs*/, uint32_t /*languages*/)
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		const AssetContainer::DataBlockInfo* settingsDataBlock = assetContainer.FindDataBlock("Settings");
		if (settingsDataBlock == nullptr)
		{
			OUTPUT_ERROR("DataTableCooker: missing 'Settings' data block");
			return false;
		}

		Document           settingsDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == false)
		{
			OUTPUT_ERROR("DataTableCooker: unable to read 'Settings' data block");
			return false;
		}

		DataTableContainer dataTableContainer;
		if (Serializer::Deserialize(dataTableContainer, settingsDocument.GetRootNode()) == false)
		{
			OUTPUT_ERROR("DataTableCooker: unable to deserialize 'Settings' data block");
			return false;
		}

		// Cooked layout: the row RttiType, a raw uint32_t row count, then that many raw uint64_t key
		// hashes (xxh3 of the row's string key), then a JSON document holding only the row data itself,
		// in the same order as the hashes. This lets the runtime resource read its type and reserve its
		// map up front, and match each row to its hash by position, with no string hashing or key/type
		// parsing needed out of JSON at Initialize time.
		const std::vector<std::pair<String, DataStruct*>>& rows = dataTableContainer.GetRows();

		Vector<uint64_t> keyHashes;
		keyHashes.Reserve(static_cast<uint32_t>(rows.size()));

		Document cookedDocument;
		DocumentNode& cookedRootNode = cookedDocument.GetRootNode();
		DocumentNode& rowsNode = cookedRootNode.AddChild("Rows");

		for (const auto& pair : rows)
		{
			keyHashes.PushBack(Hash::ComputeXxh3_64(std::string_view(pair.first)));

			if (Serializer::Serialize(static_cast<const DataStruct*>(pair.second), rowsNode.AddChild("")) == false)
			{
				OUTPUT_ERROR("DataTableCooker: unable to serialize row '{}'", pair.first);
				return false;
			}
		}

		Stream& settingsStream = AddDataBlockStream("Settings", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));

		RttiType rowType = dataTableContainer.GetRowType();
		if (settingsStream.Write(&rowType, sizeof(rowType)) != sizeof(rowType))
		{
			OUTPUT_ERROR("DataTableCooker: unable to write row type");
			return false;
		}

		uint32_t rowCount = static_cast<uint32_t>(keyHashes.Size());
		if (settingsStream.Write(&rowCount, sizeof(rowCount)) != sizeof(rowCount))
		{
			OUTPUT_ERROR("DataTableCooker: unable to write row count");
			return false;
		}

		if (rowCount > 0)
		{
			uint32_t keyHashesByteSize = rowCount * static_cast<uint32_t>(sizeof(uint64_t));
			if (settingsStream.Write(keyHashes.Data(), keyHashesByteSize) != keyHashesByteSize)
			{
				OUTPUT_ERROR("DataTableCooker: unable to write row key hashes");
				return false;
			}
		}

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(cookedDocument, settingsStream) == false)
		{
			OUTPUT_ERROR("DataTableCooker: unable to write 'Settings' data block");
			return false;
		}

		return true;
	}
}
