#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Stream/CompressionStream.hpp>
#include <HodEngine/Core/Stream/FileStream.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <string_view>

namespace hod::inline gamesystems
{
	/// @brief Binary container for cooked .res files: Header(fixed) + Data section
	class HOD_GAME_SYSTEMS_API ResourceContainer
	{
	public:
		struct DataBlockInfo
		{
			uint64_t _hashName = 0;
			Stream*  _stream = nullptr;
			uint32_t _uncompressedSize = 0;
			bool _compressed = false;
		};

	public:
		static constexpr uint32_t FORMAT_VERSION = 1;

	public:
		ResourceContainer() = default;
		~ResourceContainer() = default;

		/// @brief Read only the fixed Header, for fast scans
		bool LoadHeader(const Path& path);

		/// @brief Read the whole resource (Header + Data)
		bool Load(const Path& path);

		/// @brief Write the whole resource (Header + Data)
		bool Save(const Path& path, const Path& tmpDir);

		uint64_t GetType() const;
		void     SetType(uint64_t type);

		uint32_t GetCookerVersion() const;
		void     SetCookerVersion(uint32_t cookerVersion);

		uint64_t GetAssetContentHash() const;
		void     SetAssetContentHash(uint64_t assetContentHash);

		const Vector<DataBlockInfo>& GetDataBlocks() const;
		const DataBlockInfo*         FindDataBlock(std::string_view name) const;

		void SetDataBlock(std::string_view name, Stream& stream, bool compressed);
		void RemoveDataBlock(std::string_view name);

	private:
		bool ReadHeader(Stream& stream);

	private:
		static const uint8_t MAGIC[8];

		#pragma pack(push, 1)
		struct Header
		{
			uint8_t  magic[8] = {};
			uint32_t formatVersion = 0;
			uint64_t type = 0;
			uint64_t assetContentHash = 0;
			uint32_t cookerVersion = 0;
		};
		#pragma pack(pop)
		static_assert(sizeof(Header) == 32);

		struct DataBlockLocation
		{
			uint64_t hashName = 0;
			uint64_t position = 0;
			uint32_t size = 0;
			uint32_t uncompressedSize = 0;
			bool compressed = false;
		};

	private:
		uint64_t _type = 0;
		uint64_t _assetContentHash = 0;
		uint32_t _cookerVersion = 0;

		Vector<DataBlockInfo> _dataBlocks;
		Vector<FileStream> _internalDataBlockStream;
		Vector<CompressionStream> _internalCompressionStream;
	};
}
