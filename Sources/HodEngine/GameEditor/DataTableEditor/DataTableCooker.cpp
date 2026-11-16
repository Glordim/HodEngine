#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetData.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"
#include <HodEngine/GameSystems/Resource/ResourceVariant.hpp>

#include <algorithm>
#include <utility>

namespace hod::inline editor
{
	DataTableCooker::DataTableCooker()
	{
		SetCookerVersion(1); // 1: key hashes sorted, with their row index
	}

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

		DataTableAssetData data;
		if (data.Read(settingsDocument.GetRootNode()) == false)
		{
			OUTPUT_ERROR("DataTableCooker: unable to read 'Settings' data block");
			return false;
		}

		// Cooked layout (DataTableResource::Initialize): the row RttiType, a raw uint32_t row count, then that many raw
		// uint64_t key hashes (xxh3 of the row's string key) sorted, then the raw uint32_t row index of each of them,
		// then a JSON document holding only the rows, in the table's order. The runtime resource reads its key index
		// as is (a binary search, nothing to hash, sort or parse out of JSON at Initialize time) and keeps the order.
		const Vector<DataTableAssetData::Row>& rows = data._rows;

		Vector<std::pair<uint64_t, uint32_t>> keyHashRows; // Key hash, row index
		keyHashRows.Reserve(rows.Size());

		Document cookedDocument;
		DocumentNode& cookedRootNode = cookedDocument.GetRootNode();
		DocumentNode& rowsNode = cookedRootNode.AddChild("Rows");

		for (const DataTableAssetData::Row& row : rows)
		{
			keyHashRows.PushBack({Hash::ComputeXxh3_64(std::string_view(row._key)), keyHashRows.Size()});

			if (Serializer::Serialize(static_cast<const DataStruct*>(row._data), rowsNode.AddChild("")) == false)
			{
				OUTPUT_ERROR("DataTableCooker: unable to serialize row '{}'", row._key);
				return false;
			}
		}

		std::sort(keyHashRows.begin(), keyHashRows.end());

		Vector<uint64_t> keyHashes;
		Vector<uint32_t> rowIndices;
		keyHashes.Reserve(keyHashRows.Size());
		rowIndices.Reserve(keyHashRows.Size());
		for (const std::pair<uint64_t, uint32_t>& keyHashRow : keyHashRows)
		{
			if (keyHashes.Empty() == false && keyHashes.Back() == keyHashRow.first)
			{
				OUTPUT_ERROR("DataTableCooker: the rows '{}' and '{}' have the same key (or key hash)", rows[rowIndices.Back()]._key, rows[keyHashRow.second]._key);
				return false;
			}
			keyHashes.PushBack(keyHashRow.first);
			rowIndices.PushBack(keyHashRow.second);
		}

		Stream& settingsStream = AddDataBlockStream("Settings", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));

		RttiType rowType = data._rowType;
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

			uint32_t rowIndicesByteSize = rowCount * static_cast<uint32_t>(sizeof(uint32_t));
			if (settingsStream.Write(rowIndices.Data(), rowIndicesByteSize) != rowIndicesByteSize)
			{
				OUTPUT_ERROR("DataTableCooker: unable to write row indices");
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
