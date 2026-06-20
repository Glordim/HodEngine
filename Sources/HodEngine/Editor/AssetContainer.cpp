#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <cstring>
#include <fstream>
#include <sstream>

namespace hod::inline editor
{
	const uint8_t AssetContainer::MAGIC[8] = { 'H', 'A', 'S', 'S', 'E', 'T', '\0', '\0' };

	/// @brief Read the fixed Header followed by the SourcePath block
	/// @param handle
	/// @return
	bool AssetContainer::ReadHeader(FileSystem::Handle& handle)
	{
		uint8_t magic[8];
		if (FileSystem::GetInstance()->Read(handle, magic, sizeof(magic)) != sizeof(magic) || std::memcmp(magic, MAGIC, sizeof(MAGIC)) != 0)
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: invalid magic number for {}", handle._path);
			return false;
		}

		uint32_t formatVersion = 0;
		FileSystem::GetInstance()->Read(handle, &formatVersion, sizeof(formatVersion));
		if (formatVersion != FORMAT_VERSION)
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: unsupported format version {} for {}", formatVersion, handle._path);
			return false;
		}

		uint64_t uidLow = 0;
		uint64_t uidHigh = 0;
		FileSystem::GetInstance()->Read(handle, &uidLow, sizeof(uidLow));
		FileSystem::GetInstance()->Read(handle, &uidHigh, sizeof(uidHigh));
		_uid = UID(uidLow, uidHigh);

		FileSystem::GetInstance()->Read(handle, &_assetType, sizeof(_assetType));
		FileSystem::GetInstance()->Read(handle, &_contentHash, sizeof(_contentHash));

		FileSystem::GetInstance()->Read(handle, &_sourceHash, sizeof(_sourceHash));

		uint32_t sourcePathSize = 0;
		FileSystem::GetInstance()->Read(handle, &sourcePathSize, sizeof(sourcePathSize));

		if (sourcePathSize > 0)
		{
			String sourcePathStr;
			sourcePathStr.Resize(sourcePathSize);
			FileSystem::GetInstance()->Read(handle, sourcePathStr.Data(), sourcePathSize);
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
		FileSystem::Handle handle = FileSystem::GetInstance()->Open(path);
		if (handle.IsOpen() == false)
		{
			OUTPUT_ERROR("AssetContainer::LoadHeader: can't open {}", path);
			return false;
		}

		bool result = ReadHeader(handle);
		FileSystem::GetInstance()->Close(handle);
		return result;
	}

	/// @brief Read the whole asset (Header + SourcePath + Content + Data)
	/// @param path
	/// @return
	bool AssetContainer::Load(const Path& path)
	{
		FileSystem::Handle handle = FileSystem::GetInstance()->Open(path);
		if (handle.IsOpen() == false)
		{
			OUTPUT_ERROR("AssetContainer::Load: can't open {}", path);
			return false;
		}

		if (ReadHeader(handle) == false)
		{
			FileSystem::GetInstance()->Close(handle);
			return false;
		}

		uint32_t contentSize = 0;
		FileSystem::GetInstance()->Read(handle, &contentSize, sizeof(contentSize));

		_content.GetRootNode().Clear();
		_dataBlocks.Clear();

		if (contentSize > 0)
		{
			DocumentReaderJson reader;
			if (reader.Read(_content, handle, contentSize) == false)
			{
				OUTPUT_ERROR("AssetContainer::Load: failed to parse Content for {}", path);
				FileSystem::GetInstance()->Close(handle);
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

		uint32_t fileSize = FileSystem::GetInstance()->GetSize(handle);
		uint32_t currentOffset = FileSystem::GetInstance()->GetOffset(handle);
		uint32_t dataSize = (fileSize > currentOffset) ? (fileSize - currentOffset) : 0;

		_data.Clear();
		_data.Resize(dataSize);
		if (dataSize > 0)
		{
			FileSystem::GetInstance()->Read(handle, _data.Data(), dataSize);
		}

		FileSystem::GetInstance()->Close(handle);
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

		std::ostringstream contentStream;
		DocumentWriterJson contentWriter;
		if (contentWriter.Write(_content, contentStream) == false)
		{
			OUTPUT_ERROR("AssetContainer::Save: failed to serialize Content for {}", path);
			return false;
		}
		std::string contentStr = contentStream.str();

		const String& sourcePathStr = _sourcePath.GetString();
		uint32_t      sourcePathSize = sourcePathStr.Size();
		uint32_t      contentSize = static_cast<uint32_t>(contentStr.size());

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
		std::memcpy(hashCursor, contentStr.data(), contentSize);
		hashCursor += contentSize;
		if (_data.Empty() == false)
		{
			std::memcpy(hashCursor, _data.Data(), _data.Size());
		}

		_contentHash = Hash::ComputeFnv64(std::string_view(reinterpret_cast<const char*>(hashBuffer.Data()), hashBuffer.Size()));

		std::ofstream file(path.GetString().CStr(), std::ios::binary | std::ios::trunc);
		if (file.is_open() == false)
		{
			OUTPUT_ERROR("AssetContainer::Save: can't open {}", path);
			return false;
		}

		file.write(reinterpret_cast<const char*>(MAGIC), sizeof(MAGIC));

		uint32_t formatVersion = FORMAT_VERSION;
		file.write(reinterpret_cast<const char*>(&formatVersion), sizeof(formatVersion));

		uint64_t uidLow = _uid.GetLow();
		uint64_t uidHigh = _uid.GetHigh();
		file.write(reinterpret_cast<const char*>(&uidLow), sizeof(uidLow));
		file.write(reinterpret_cast<const char*>(&uidHigh), sizeof(uidHigh));

		file.write(reinterpret_cast<const char*>(&_assetType), sizeof(_assetType));
		file.write(reinterpret_cast<const char*>(&_contentHash), sizeof(_contentHash));

		file.write(reinterpret_cast<const char*>(&_sourceHash), sizeof(_sourceHash));
		file.write(reinterpret_cast<const char*>(&sourcePathSize), sizeof(sourcePathSize));
		if (sourcePathSize > 0)
		{
			file.write(sourcePathStr.CStr(), sourcePathSize);
		}

		file.write(reinterpret_cast<const char*>(&contentSize), sizeof(contentSize));
		if (contentSize > 0)
		{
			file.write(contentStr.data(), contentSize);
		}

		if (_data.Empty() == false)
		{
			file.write(reinterpret_cast<const char*>(_data.Data()), _data.Size());
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
