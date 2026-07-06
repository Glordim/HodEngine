#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace hod::inline editor
{
	const uint8_t AssetContainer::MAGIC[8] = { 'H', 'A', 'S', 'S', 'E', 'T', '\0', '\0' };

	/// @brief Read the fixed Header followed by the SourcePath block
	/// @param handle
	/// @return
	bool AssetContainer::ReadHeader(Stream& stream)
	{
		Header header;
		if (stream.Read(&header, sizeof(header)) != sizeof(header))
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: read failed");
			return false;
		}

		if (std::memcmp(header.magic, MAGIC, sizeof(MAGIC)) != 0)
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: invalid magic number");
			return false;
		}

		if (header.formatVersion != FORMAT_VERSION)
		{
			OUTPUT_ERROR("AssetContainer::ReadHeader: unsupported format version {}", header.formatVersion);
			return false;
		}

		_uid = header.uid;
		_assetType = header.assetType;
		_contentHash = header.contentHash;

		return true;
	}

	bool AssetContainer::ReadSource(Stream& stream)
	{
		uint32_t sourcePathSize = 0;
		stream.Read(&sourcePathSize, sizeof(sourcePathSize));

		if (sourcePathSize > 0)
		{
			String sourcePathStr;
			sourcePathStr.Resize(sourcePathSize);
			stream.Read(sourcePathStr.Data(), sourcePathSize);
			_sourcePath = sourcePathStr;
			stream.Read(&_sourceHash, sizeof(_sourceHash));
		}
		else
		{
			_sourcePath.Clear();
			_sourceHash = 0;
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

	bool AssetContainer::LoadSource(const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("AssetContainer::LoadHeader: can't open {}", path);
			return false;
		}

		return ReadHeader(fileStream) && ReadSource(fileStream);
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

		if (ReadSource(fileStream) == false)
		{
			return false;
		}

		uint32_t importSettingsLen = 0;
		fileStream.Read(&importSettingsLen, sizeof(importSettingsLen));
		// Always 0 for now

		uint32_t dataBlockCount = 0;
		fileStream.Read(&dataBlockCount, sizeof(dataBlockCount));

		Vector<DataBlockLocation> dataBlockLocation;
		dataBlockLocation.Resize(dataBlockCount);
		_internalDataBlockStream.Resize(dataBlockCount);
		_dataBlocks.Resize(dataBlockCount);
		fileStream.Read(dataBlockLocation.Data(), sizeof(DataBlockLocation) * dataBlockCount);

		uint32_t compressedCount = 0;
		for (uint32_t i = 0; i < dataBlockCount; ++i)
		{
			if (dataBlockLocation[i].compressed)
			{
				++compressedCount;
			}
		}
		_internalCompressionStream.Reserve(compressedCount);

		uint32_t fileSize = fileStream.GetSize();
		for (uint32_t i = 0; i < dataBlockCount; ++i)
		{
			uint32_t blockEnd = (i + 1 < dataBlockCount) ? dataBlockLocation[i + 1].position : fileSize;
			_internalDataBlockStream[i].Open(path);
			_internalDataBlockStream[i].SetRange(dataBlockLocation[i].position, blockEnd - dataBlockLocation[i].position);

			_dataBlocks[i]._hashName = dataBlockLocation[i].hashName;
			_dataBlocks[i]._compressed = dataBlockLocation[i].compressed;

			if (dataBlockLocation[i].compressed)
			{
				_internalCompressionStream.EmplaceBack(_internalDataBlockStream[i]);
				_dataBlocks[i]._stream = &_internalCompressionStream.Back();
			}
			else
			{
				_dataBlocks[i]._stream = &_internalDataBlockStream[i];
			}
		}
		return true;
	}

	/// @brief Write the whole asset (Header + SourcePath + Content + Data)
	/// @param path
	/// @return
	bool AssetContainer::Save(const Path& path, const Path& tmpDir)
	{
		Path tmpFile = tmpDir / path.Filename();

		FileStream file;
		if (file.Open(tmpFile, FileSystem::OpenMode::Write) == false)
		{
			OUTPUT_ERROR("AssetContainer::Save: can't open {}", path);
			return false;
		}

		Header header;
		std::memcpy(header.magic, MAGIC, sizeof(MAGIC));
		header.formatVersion = FORMAT_VERSION;
		header.uid = _uid;
		header.assetType = _assetType;
		header.contentHash = 0; // tmp, real value will be write at the end
		file.Write(&header, sizeof(header));

		uint32_t sourcePathLen = _sourcePath.GetString().Size();
		file.Write(&sourcePathLen, sizeof(sourcePathLen));
		if (sourcePathLen > 0)
		{
			file.Write(_sourcePath.CStr(), sourcePathLen);
			file.Write(&_sourceHash, sizeof(_sourceHash));
		}

		uint32_t importSettingsLen = 0;
		file.Write(&importSettingsLen, sizeof(importSettingsLen));

		uint32_t dataBlockCount = _dataBlocks.Size();
		file.Write(&dataBlockCount, sizeof(dataBlockCount));

		if (dataBlockCount > 0)
		{
			uint32_t tablePosition = file.GetPosition();
			Vector<DataBlockLocation> dataBlockLocations;
			dataBlockLocations.Resize(dataBlockCount);
			file.Write(dataBlockLocations.Data(), sizeof(DataBlockLocation) * dataBlockCount);

			void* hashState = nullptr;
			uint8_t buffer[256 * 1024];
			for (uint32_t i = 0; i < dataBlockCount; ++i)
			{
				dataBlockLocations[i].hashName = _dataBlocks[i]._hashName;
				dataBlockLocations[i].position = file.GetPosition();
				dataBlockLocations[i].compressed = _dataBlocks[i]._compressed;

				Stream& realStream = _dataBlocks[i]._compressed ? *static_cast<CompressionStream*>(_dataBlocks[i]._stream)->GetRealStream() : *_dataBlocks[i]._stream;
				realStream.Seek(0, Stream::SeekOrigin::Begin);
				uint32_t readBytes = 0;
				while (true)
				{
					readBytes = realStream.Read(buffer, sizeof(buffer));
					if (readBytes > 0)
					{
						file.Write(buffer, readBytes);
						hashState = Hash::ComputeXxh3_64_Cumulated(hashState, buffer, readBytes);
						if (readBytes != sizeof(buffer))
						{
							break;
						}
					}
					else
					{
						OUTPUT_ERROR("AssetContainer::Save: unable to read datablock");
						return false;
					}
				}
			}

			file.Seek(tablePosition, Stream::SeekOrigin::Begin);
			file.Write(dataBlockLocations.Data(), sizeof(DataBlockLocation) * dataBlockCount);
			_contentHash = Hash::ComputeXxh3_64_Result(hashState);
		}
		file.Seek(offsetof(Header, contentHash), Stream::SeekOrigin::Begin);
		file.Write(&_contentHash, sizeof(_contentHash));

		file.Close();
		return FileSystem::GetInstance()->Rename(tmpFile, path);
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

	const Vector<AssetContainer::DataBlockInfo>& AssetContainer::GetDataBlocks() const
	{
		return _dataBlocks;
	}

	const AssetContainer::DataBlockInfo* AssetContainer::FindDataBlock(std::string_view name) const
	{
		uint64_t hashName = Hash::ComputeXxh3_64(name.data(), name.size());

		for (const DataBlockInfo& info : _dataBlocks)
		{
			if (info._hashName == hashName)
			{
				return &info;
			}
		}
		return nullptr;
	}

	void AssetContainer::SetDataBlock(std::string_view name, Stream& stream, bool compressed)
	{
		RemoveDataBlock(name);

		DataBlockInfo info;
		info._hashName = Hash::ComputeXxh3_64(name.data(), name.size());
		info._stream = &stream;
		info._compressed = compressed;

		_dataBlocks.PushBack(std::move(info));
	}

	void AssetContainer::RemoveDataBlock(std::string_view name)
	{
		uint64_t hashName = Hash::ComputeXxh3_64(name.data(), name.size());

		for (uint32_t i = 0; i < _dataBlocks.Size(); ++i)
		{
			if (_dataBlocks[i]._hashName == hashName)
			{
				_dataBlocks.Erase(i);
				return;
			}
		}
	}
}
