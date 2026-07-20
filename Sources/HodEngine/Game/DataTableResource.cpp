#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataTableResource.hpp"
#include "HodEngine/Game/DataStruct.hpp"
#include "HodEngine/Game/DataStructFactory.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

#include <vector>

namespace hod::inline game
{
	DataTableResource::~DataTableResource()
	{
		for (auto& pair : _rows)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief
	/// @return
	bool DataTableResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* settingsDataBlock = resourceContainer.FindDataBlock("Settings");
		if (settingsDataBlock == nullptr)
		{
			OUTPUT_ERROR("DataTableResource::Initialize: missing 'Settings' data block");
			return false;
		}

		Stream& stream = *settingsDataBlock->_stream;

		if (stream.Read(&_rowType, sizeof(_rowType)) != sizeof(_rowType))
		{
			OUTPUT_ERROR("DataTableResource::Initialize: unable to read row type");
			return false;
		}

		uint32_t rowCount = 0;
		if (stream.Read(&rowCount, sizeof(rowCount)) != sizeof(rowCount))
		{
			OUTPUT_ERROR("DataTableResource::Initialize: unable to read row count");
			return false;
		}

		std::vector<uint64_t> keyHashes(rowCount);
		if (rowCount > 0)
		{
			uint32_t keyHashesByteSize = rowCount * static_cast<uint32_t>(sizeof(uint64_t));
			if (stream.Read(keyHashes.data(), keyHashesByteSize) != keyHashesByteSize)
			{
				OUTPUT_ERROR("DataTableResource::Initialize: unable to read row key hashes");
				return false;
			}
		}

		Document           settingsDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(settingsDocument, stream, stream.GetSize() - stream.GetPosition()) == false)
		{
			OUTPUT_ERROR("DataTableResource::Initialize: unable to read 'Settings' data block");
			return false;
		}

		const DocumentNode& rootNode = settingsDocument.GetRootNode();

		ReflectionDescriptor* rowReflectionDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(_rowType);
		if (rowReflectionDescriptor == nullptr)
		{
			OUTPUT_ERROR("DataTableResource::Initialize: unknown row type {}", _rowType);
			return false;
		}

		_rows.reserve(rowCount);

		const DocumentNode* rowsNode = rootNode.GetChild("Rows");
		if (rowsNode != nullptr)
		{
			const DocumentNode* rowNode = rowsNode->GetFirstChild();
			uint32_t            index = 0;
			while (rowNode != nullptr && index < rowCount)
			{
				DataStruct* dataStruct = static_cast<DataStruct*>(rowReflectionDescriptor->CreateInstance());
				if (Serializer::Deserialize(*dataStruct, *rowNode) == false)
				{
					DefaultAllocator::GetInstance().Delete(dataStruct);
					OUTPUT_ERROR("DataTableResource::Initialize: unable to deserialize row {}", index);
					return false;
				}

				_rows.emplace(keyHashes[index], dataStruct);

				rowNode = rowNode->GetNextSibling();
				++index;
			}
		}

		return true;
	}

	/// @brief
	/// @return
	RttiType DataTableResource::GetRowType() const
	{
		return _rowType;
	}

	/// @brief
	/// @param keyHash
	/// @return
	DataStruct* DataTableResource::FindRow(uint64_t keyHash) const
	{
		auto it = _rows.find(keyHash);
		if (it != _rows.end())
		{
			return it->second;
		}
		return nullptr;
	}

	/// @brief
	/// @param key
	/// @return
	DataStruct* DataTableResource::FindRow(std::string_view key) const
	{
		return FindRow(Hash::ComputeXxh3_64(key));
	}

	/// @brief
	/// @return
	const std::unordered_map<uint64_t, DataStruct*>& DataTableResource::GetRows() const
	{
		return _rows;
	}
}
