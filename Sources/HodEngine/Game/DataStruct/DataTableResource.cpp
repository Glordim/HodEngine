#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataStruct/DataTableResource.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include "HodEngine/Core/Assert.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(DataTableResource) {}

	DataTableResource::~DataTableResource()
	{
		if (_rows == nullptr)
		{
			return;
		}

		for (uint32_t index = 0; index < _rowCount; ++index)
		{
			_rowDescriptor->DestroyAt(_rows + index * _rowStride);
		}
		DefaultAllocator::GetInstance().Free(_rows);
	}

	/// @brief Cooked layout (DataTableCooker): the row RttiType, the row count, the key hashes sorted, the row index of each of them,
	/// then a JSON document holding the rows in the table's order
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

		if (rowCount > 0)
		{
			_sortedKeyHashes.Resize(rowCount);
			uint32_t keyHashesByteSize = rowCount * static_cast<uint32_t>(sizeof(uint64_t));
			if (stream.Read(_sortedKeyHashes.Data(), keyHashesByteSize) != keyHashesByteSize)
			{
				OUTPUT_ERROR("DataTableResource::Initialize: unable to read row key hashes");
				return false;
			}

			_rowIndexOfSortedKeys.Resize(rowCount);
			uint32_t rowIndicesByteSize = rowCount * static_cast<uint32_t>(sizeof(uint32_t));
			if (stream.Read(_rowIndexOfSortedKeys.Data(), rowIndicesByteSize) != rowIndicesByteSize)
			{
				OUTPUT_ERROR("DataTableResource::Initialize: unable to read row indices");
				return false;
			}

			for (uint32_t index = 0; index < rowCount; ++index)
			{
				if ((index > 0 && _sortedKeyHashes[index - 1] >= _sortedKeyHashes[index]) || _rowIndexOfSortedKeys[index] >= rowCount)
				{
					OUTPUT_ERROR("DataTableResource::Initialize: its key hashes aren't sorted, or a row index is out of range");
					return false;
				}
			}
		}

		Document           settingsDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(settingsDocument, stream, stream.GetSize() - stream.GetPosition()) == false)
		{
			OUTPUT_ERROR("DataTableResource::Initialize: unable to read 'Settings' data block");
			return false;
		}

		_rowDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(_rowType);
		if (_rowDescriptor == nullptr)
		{
			OUTPUT_ERROR("DataTableResource::Initialize: unknown row type {}", _rowType);
			return false;
		}
		if (_rowDescriptor->CanCreateInstance() == false)
		{
			OUTPUT_ERROR("DataTableResource::Initialize: its row type {} can't be constructed", _rowDescriptor->GetTypeName());
			return false;
		}

		if (rowCount == 0)
		{
			return true;
		}

		// Every row constructed first: the destructor destroys _rowCount of them whatever fails next
		_rowStride = _rowDescriptor->GetSize(); // sizeof: a multiple of its alignment
		_rows = static_cast<uint8_t*>(DefaultAllocator::GetInstance().Allocate(rowCount * _rowStride, _rowDescriptor->GetAlignment()));
		for (uint32_t index = 0; index < rowCount; ++index)
		{
			_rowDescriptor->ConstructAt(_rows + index * _rowStride);
		}
		_rowCount = rowCount;

		const DocumentNode* rowsNode = settingsDocument.GetRootNode().GetChild("Rows");
		const DocumentNode* rowNode = (rowsNode != nullptr) ? rowsNode->GetFirstChild() : nullptr;
		for (uint32_t index = 0; index < rowCount; ++index)
		{
			if (rowNode == nullptr)
			{
				OUTPUT_ERROR("DataTableResource::Initialize: {} rows written, {} expected", index, rowCount);
				return false;
			}

			if (Serializer::Deserialize(*_rowDescriptor, _rows + index * _rowStride, *rowNode) == false)
			{
				OUTPUT_ERROR("DataTableResource::Initialize: unable to deserialize row {}", index);
				return false;
			}

			rowNode = rowNode->GetNextSibling();
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
	/// @return
	uint32_t DataTableResource::GetRowCount() const
	{
		return _rowCount;
	}

	/// @brief
	/// @param index
	/// @return
	const DataStruct* DataTableResource::GetRow(uint32_t index) const
	{
		Assert(index < _rowCount);
		return static_cast<const DataStruct*>(static_cast<const void*>(_rows + index * _rowStride));
	}

	/// @brief
	/// @param keyHash
	/// @return
	const DataStruct* DataTableResource::FindRow(uint64_t keyHash) const
	{
		// Binary search, the cooker sorted them
		uint32_t first = 0;
		uint32_t last = _sortedKeyHashes.Size();
		while (first < last)
		{
			uint32_t middle = first + (last - first) / 2;
			if (_sortedKeyHashes[middle] < keyHash)
			{
				first = middle + 1;
			}
			else
			{
				last = middle;
			}
		}

		if (first < _sortedKeyHashes.Size() && _sortedKeyHashes[first] == keyHash)
		{
			return GetRow(_rowIndexOfSortedKeys[first]);
		}
		return nullptr;
	}

	/// @brief
	/// @param key
	/// @return
	const DataStruct* DataTableResource::FindRow(std::string_view key) const
	{
		return FindRow(Hash::ComputeXxh3_64(key));
	}

	/// @brief
	/// @param type
	/// @return
	bool DataTableResource::CheckRowType(RttiType type) const
	{
		Assert(type == _rowType);
		return type == _rowType;
	}
}
