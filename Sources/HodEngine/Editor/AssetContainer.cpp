#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"

#include <cstring>

namespace hod::inline editor
{
	const uint8_t AssetContainer::MAGIC[8] = { 'H', 'A', 'S', 'S', 'E', 'T', '\0', '\0' };

	/// @brief Read the fixed Header followed by the SourcePath block
	/// @param handle
	/// @return
	bool AssetContainer::ReadHeader(Stream& stream)
	{
		uint8_t magic[8];
		if (stream.Read(magic, sizeof(magic)) != sizeof(magic) || std::memcmp(magic, MAGIC, sizeof(MAGIC)) != 0)
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: invalid magic number");
			return false;
		}

		uint32_t formatVersion = 0;
		stream.Read(&formatVersion, sizeof(formatVersion));
		if (formatVersion != FORMAT_VERSION)
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: unsupported format version {}", formatVersion);
			return false;
		}

		uint64_t uidLow = 0;
		uint64_t uidHigh = 0;
		stream.Read(&uidLow, sizeof(uidLow));
		stream.Read(&uidHigh, sizeof(uidHigh));
		_uid = UID(uidLow, uidHigh);

		stream.Read(&_assetType, sizeof(_assetType));
		stream.Read(&_contentHash, sizeof(_contentHash));

		stream.Read(&_sourceHash, sizeof(_sourceHash));

		uint32_t sourcePathSize = 0;
		stream.Read(&sourcePathSize, sizeof(sourcePathSize));

		if (sourcePathSize > 0)
		{
			String sourcePathStr;
			sourcePathStr.Resize(sourcePathSize);
			stream.Read(sourcePathStr.Data(), sourcePathSize);
			_sourcePath = sourcePathStr;
		}
		else
		{
			_sourcePath.Clear();
		}

		return true;
	}

	/// @brief Read only the fixed Header and the SourcePath block (no Content/Data), for fast scans at startup
	/// @param path
	/// @return
	bool AssetContainer::LoadHeader(const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("AssetContainer::LoadHeader: can't open {}", path);
			return false;
		}

		return ReadHeader(fileStream);
	}

	/// @brief Read the whole asset (Header + SourcePath + Content + Data)
	/// @param path
	/// @return
	bool AssetContainer::Load(const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("AssetContainer::Load: can't open {}", path);
			return false;
		}

		if (ReadHeader(fileStream) == false)
		{
			return false;
		}

		uint32_t contentSize = 0;
		fileStream.Read(&contentSize, sizeof(contentSize));

		_content.GetRootNode().Clear();
		_dataBlocks.Clear();

		if (contentSize > 0)
		{
			DocumentReaderJson reader;
			if (reader.Read(_content, fileStream, contentSize) == false)
			{
				OUTPUT_ERROR("AssetContainer::Load: failed to parse Content for {}", path);
				return false;
			}

			const DocumentNode* dataBlocksNode = _content.GetRootNode().GetChild("_dataBlocks");
			if (dataBlocksNode != nullptr)
			{
				for (DocumentNode* child = dataBlocksNode->GetFirstChild(); child != nullptr; child = child->GetNextSibling())
				{
					DataBlockInfo info;

					const DocumentNode* nameNode = child->GetChild("_name");
					const DocumentNode* offsetNode = child->GetChild("_offset");
					const DocumentNode* sizeNode = child->GetChild("_size");

					if (nameNode != nullptr)
					{
						info._name = nameNode->GetString();
					}
					if (offsetNode != nullptr)
					{
						info._offset = offsetNode->GetUInt32();
					}
					if (sizeNode != nullptr)
					{
						info._size = sizeNode->GetUInt32();
					}

					_dataBlocks.PushBack(std::move(info));
				}
			}
		}

		uint32_t fileSize = fileStream.GetSize();
		uint32_t currentOffset = fileStream.GetPosition();
		uint32_t dataSize = (fileSize > currentOffset) ? (fileSize - currentOffset) : 0;

		_data.Clear();
		_data.Resize(dataSize);
		if (dataSize > 0)
		{
			fileStream.Read(_data.Data(), dataSize);
		}

		return true;
	}

	/// @brief Write the whole asset (Header + SourcePath + Content + Data)
	/// @param path
	/// @return
	bool AssetContainer::Save(const Path& path)
	{
		DocumentNode& dataBlocksNode = _content.GetRootNode().GetOrAddChild("_dataBlocks");
		dataBlocksNode.Clear();
		dataBlocksNode.SetType(DocumentNode::Type::Array);

		uint32_t cursor = 0;
		for (DataBlockInfo& info : _dataBlocks)
		{
			info._offset = cursor;
			cursor += info._size;

			DocumentNode& entryNode = dataBlocksNode.AddChild("");
			entryNode.SetType(DocumentNode::Type::Object);
			entryNode.GetOrAddChild("_name").SetString(info._name);
			entryNode.GetOrAddChild("_offset").SetUInt32(info._offset);
			entryNode.GetOrAddChild("_size").SetUInt32(info._size);
		}

		String contentStr;
		DocumentWriterJson contentWriter;
		if (contentWriter.Write(_content, contentStr) == false)
		{
			OUTPUT_ERROR("AssetContainer::Save: failed to serialize Content for {}", path);
			return false;
		}

		const String& sourcePathStr = _sourcePath.GetString();
		uint32_t      sourcePathSize = sourcePathStr.Size();
		uint32_t      contentSize = contentStr.Size();

		// ContentHash = Fnv64(SourceHash + SourcePathSize + SourcePath + ContentSize + Content + Data)
		Vector<uint8_t> hashBuffer;
		hashBuffer.Resize(sizeof(_sourceHash) + sizeof(sourcePathSize) + sourcePathSize + sizeof(contentSize) + contentSize + _data.Size());

		uint8_t* hashCursor = hashBuffer.Data();
		std::memcpy(hashCursor, &_sourceHash, sizeof(_sourceHash));
		hashCursor += sizeof(_sourceHash);
		std::memcpy(hashCursor, &sourcePathSize, sizeof(sourcePathSize));
		hashCursor += sizeof(sourcePathSize);
		std::memcpy(hashCursor, sourcePathStr.CStr(), sourcePathSize);
		hashCursor += sourcePathSize;
		std::memcpy(hashCursor, &contentSize, sizeof(contentSize));
		hashCursor += sizeof(contentSize);
		std::memcpy(hashCursor, contentStr.CStr(), contentSize);
		hashCursor += contentSize;
		if (_data.Empty() == false)
		{
			std::memcpy(hashCursor, _data.Data(), _data.Size());
		}

		_contentHash = Hash::ComputeFnv64(std::string_view(reinterpret_cast<const char*>(hashBuffer.Data()), hashBuffer.Size()));

		FileStream file;
		if (file.Open(path, FileSystem::OpenMode::Write) == false)
		{
			OUTPUT_ERROR("AssetContainer::Save: can't open {}", path);
			return false;
		}

		file.Write(MAGIC, sizeof(MAGIC));

		uint32_t formatVersion = FORMAT_VERSION;
		file.Write(&formatVersion, sizeof(formatVersion));

		uint64_t uidLow = _uid.GetLow();
		uint64_t uidHigh = _uid.GetHigh();
		file.Write(&uidLow, sizeof(uidLow));
		file.Write(&uidHigh, sizeof(uidHigh));

		file.Write(&_assetType, sizeof(_assetType));
		file.Write(&_contentHash, sizeof(_contentHash));

		file.Write(&_sourceHash, sizeof(_sourceHash));
		file.Write(&sourcePathSize, sizeof(sourcePathSize));
		if (sourcePathSize > 0)
		{
			file.Write(sourcePathStr.CStr(), sourcePathSize);
		}

		file.Write(&contentSize, sizeof(contentSize));
		if (contentSize > 0)
		{
			file.Write(contentStr.CStr(), contentSize);
		}

		if (_data.Empty() == false)
		{
			file.Write(_data.Data(), _data.Size());
		}

		return true;
	}

	const UID& AssetContainer::GetUid() const
	{
		return _uid;
	}

	void AssetContainer::SetUid(const UID& uid)
	{
		_uid = uid;
	}

	uint64_t AssetContainer::GetAssetType() const
	{
		return _assetType;
	}

	void AssetContainer::SetAssetType(uint64_t assetType)
	{
		_assetType = assetType;
	}

	uint64_t AssetContainer::GetContentHash() const
	{
		return _contentHash;
	}

	bool AssetContainer::HasSourcePath() const
	{
		return _sourcePath.Empty() == false;
	}

	const Path& AssetContainer::GetSourcePath() const
	{
		return _sourcePath;
	}

	void AssetContainer::SetSourcePath(const Path& sourcePath)
	{
		_sourcePath = sourcePath;
	}

	uint64_t AssetContainer::GetSourceHash() const
	{
		return _sourceHash;
	}

	void AssetContainer::SetSourceHash(uint64_t sourceHash)
	{
		_sourceHash = sourceHash;
	}

	DocumentNode& AssetContainer::GetImportSettings()
	{
		return _importSettings.GetRootNode();
	}

	const DocumentNode& AssetContainer::GetImportSettings() const
	{
		return _importSettings.GetRootNode();
	}

	DocumentNode& AssetContainer::GetContentRoot()
	{
		return _content.GetRootNode();
	}

	const DocumentNode& AssetContainer::GetContentRoot() const
	{
		return _content.GetRootNode();
	}

	const Vector<AssetContainer::DataBlockInfo>& AssetContainer::GetDataBlocks() const
	{
		return _dataBlocks;
	}

	const AssetContainer::DataBlockInfo* AssetContainer::FindDataBlock(std::string_view name) const
	{
		for (const DataBlockInfo& info : _dataBlocks)
		{
			if (std::string_view(info._name.CStr(), info._name.Size()) == name)
			{
				return &info;
			}
		}
		return nullptr;
	}

	const uint8_t* AssetContainer::GetDataBlockBuffer(const DataBlockInfo& dataBlock) const
	{
		return _data.Data() + dataBlock._offset;
	}

	void AssetContainer::SetDataBlock(std::string_view name, const void* buffer, uint32_t size)
	{
		RemoveDataBlock(name);

		DataBlockInfo info;
		info._name = String(name);
		info._offset = _data.Size();
		info._size = size;

		uint32_t oldSize = _data.Size();
		_data.Resize(oldSize + size);
		if (size > 0)
		{
			std::memcpy(_data.Data() + oldSize, buffer, size);
		}

		_dataBlocks.PushBack(std::move(info));
	}

	void AssetContainer::RemoveDataBlock(std::string_view name)
	{
		for (uint32_t i = 0; i < _dataBlocks.Size(); ++i)
		{
			if (std::string_view(_dataBlocks[i]._name.CStr(), _dataBlocks[i]._name.Size()) == name)
			{
				DataBlockInfo removed = _dataBlocks[i];

				_data.Erase(_data.Begin() + removed._offset, _data.Begin() + removed._offset + removed._size);

				for (uint32_t j = 0; j < _dataBlocks.Size(); ++j)
				{
					if (_dataBlocks[j]._offset > removed._offset)
					{
						_dataBlocks[j]._offset -= removed._size;
					}
				}

				_dataBlocks.Erase(i);
				return;
			}
		}
	}
}
