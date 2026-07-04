#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace hod::inline gamesystems
{
	const uint8_t ResourceContainer::MAGIC[8] = { 'H', 'R', 'E', 'S', 'R', 'C', '\0', '\0' };

	/// @brief Read the fixed Header
	/// @param stream
	/// @return
	bool ResourceContainer::ReadHeader(Stream& stream)
	{
		Header header;
		if (stream.Read(&header, sizeof(header)) != sizeof(header))
		{
			OUTPUT_ERROR("ResourceContainer::ReadHeader: read failed");
			return false;
		}

		if (std::memcmp(header.magic, MAGIC, sizeof(MAGIC)) != 0)
		{
			OUTPUT_ERROR("ResourceContainer::ReadHeader: invalid magic number");
			return false;
		}

		if (header.formatVersion != FORMAT_VERSION)
		{
			OUTPUT_ERROR("ResourceContainer::ReadHeader: unsupported format version {}", header.formatVersion);
			return false;
		}

		_type = header.type;
		_assetContentHash = header.assetContentHash;
		_cookerVersion = header.cookerVersion;

		return true;
	}

	/// @brief Read only the fixed Header, for fast scans
	/// @param path
	/// @return
	bool ResourceContainer::LoadHeader(const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("ResourceContainer::LoadHeader: can't open {}", path);
			return false;
		}

		return ReadHeader(fileStream);
	}

	/// @brief Read the whole resource (Header + Data)
	/// @param path
	/// @return
	bool ResourceContainer::Load(const Path& path)
	{
		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("ResourceContainer::Load: can't open {}", path);
			return false;
		}

		if (ReadHeader(fileStream) == false)
		{
			return false;
		}

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

	/// @brief Write the whole resource (Header + Data)
	/// @param path
	/// @param tmpDir
	/// @return
	bool ResourceContainer::Save(const Path& path, const Path& tmpDir)
	{
		Path tmpFile = tmpDir / path.Filename();

		FileStream file;
		if (file.Open(tmpFile, FileSystem::OpenMode::Write) == false)
		{
			OUTPUT_ERROR("ResourceContainer::Save: can't open {}", path);
			return false;
		}

		Header header;
		std::memcpy(header.magic, MAGIC, sizeof(MAGIC));
		header.formatVersion = FORMAT_VERSION;
		header.type = _type;
		header.assetContentHash = _assetContentHash;
		header.cookerVersion = _cookerVersion;
		file.Write(&header, sizeof(header));

		uint32_t dataBlockCount = _dataBlocks.Size();
		file.Write(&dataBlockCount, sizeof(dataBlockCount));

		uint32_t tablePosition = file.GetPosition();
		Vector<DataBlockLocation> dataBlockLocations;
		dataBlockLocations.Resize(dataBlockCount);
		file.Write(dataBlockLocations.Data(), sizeof(DataBlockLocation) * dataBlockCount);

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
					if (readBytes != sizeof(buffer))
					{
						break;
					}
				}
				else
				{
					OUTPUT_ERROR("ResourceContainer::Save: unable to read datablock");
					return false;
				}
			}
		}

		file.Seek(tablePosition, Stream::SeekOrigin::Begin);
		file.Write(dataBlockLocations.Data(), sizeof(DataBlockLocation) * dataBlockCount);

		file.Close();
		return FileSystem::GetInstance()->Rename(tmpFile, path);
	}

	uint64_t ResourceContainer::GetType() const
	{
		return _type;
	}

	void ResourceContainer::SetType(uint64_t type)
	{
		_type = type;
	}

	uint32_t ResourceContainer::GetCookerVersion() const
	{
		return _cookerVersion;
	}

	void ResourceContainer::SetCookerVersion(uint32_t cookerVersion)
	{
		_cookerVersion = cookerVersion;
	}

	uint64_t ResourceContainer::GetAssetContentHash() const
	{
		return _assetContentHash;
	}

	void ResourceContainer::SetAssetContentHash(uint64_t assetContentHash)
	{
		_assetContentHash = assetContentHash;
	}

	const Vector<ResourceContainer::DataBlockInfo>& ResourceContainer::GetDataBlocks() const
	{
		return _dataBlocks;
	}

	const ResourceContainer::DataBlockInfo* ResourceContainer::FindDataBlock(std::string_view name) const
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

	void ResourceContainer::SetDataBlock(std::string_view name, Stream& stream, bool compressed)
	{
		RemoveDataBlock(name);

		DataBlockInfo info;
		info._hashName = Hash::ComputeXxh3_64(name.data(), name.size());
		info._stream = &stream;
		info._compressed = compressed;

		_dataBlocks.PushBack(std::move(info));
	}

	void ResourceContainer::RemoveDataBlock(std::string_view name)
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
