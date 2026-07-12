#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Stream/CompressionStream.hpp>
#include <HodEngine/Core/Stream/FileStream.hpp>
#include <HodEngine/Core/Stream/MemoryStream.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <string_view>

namespace hod::inline editor
{
	/// @brief Binary container for .asset files: Header(fixed) + Sources block + Content block (json) + Data section
	class HOD_EDITOR_API AssetContainer
	{
	public:
		struct DataBlockInfo
		{
			uint64_t _hashName = 0;
			Stream*  _stream = nullptr;
			uint32_t _uncompressedSize = 0;
			bool     _compressed = false;
		};

		struct SourceInfo
		{
			Path     _path;
			uint64_t _hash = 0;
		};

	public:
		static constexpr uint32_t FORMAT_VERSION = 2;

	public:
		AssetContainer() = default;
		~AssetContainer();

		/// @brief Read only the fixed Header and the Sources block (no Content/Data), for fast scans at startup
		bool LoadHeader(const Path& path);
		bool LoadSources(const Path& path);

		/// @brief Read the whole asset (Header + Sources + Content + Data)
		bool Load(const Path& path);

		/// @brief Write the whole asset (Header + Sources + Content + Data)
		bool Save(const Path& path, const Path& tmpDir = Path());

		const UID& GetUid() const;
		void       SetUid(const UID& uid);

		uint64_t GetAssetType() const;
		void     SetAssetType(uint64_t assetType);

		uint64_t GetContentHash() const;

		bool                        HasSources() const;
		const Vector<SourceInfo>&   GetSources() const;
		void                        SetSources(Vector<SourceInfo> sources);
		void                        AddSource(const Path& sourcePath, uint64_t sourceHash);
		void                        ClearSources();

		DocumentNode&       GetImportSettings();
		const DocumentNode& GetImportSettings() const;

		const Vector<DataBlockInfo>& GetDataBlocks() const;
		const DataBlockInfo*         FindDataBlock(std::string_view name) const;

		void    SetDataBlock(std::string_view name, Stream& stream, bool compressed);
		Stream& AddDataBlock(std::string_view name, bool compressed);
		void    RemoveDataBlock(std::string_view name);
		void    ClearDataBlocks();

	private:
		bool ReadHeader(Stream& stream);
		bool ReadSources(Stream& stream);

	private:
		static const uint8_t MAGIC[8];

#pragma pack(push, 1)

		struct Header
		{
			uint8_t  magic[8] = {};
			uint32_t formatVersion = 0;
			UID      uid;
			uint64_t assetType = 0;
			uint64_t contentHash = 0;
		};

#pragma pack(pop)
		static_assert(sizeof(Header) == 44);

		struct DataBlockLocation
		{
			uint64_t hashName = 0;
			uint64_t position = 0;
			uint32_t size = 0;
			uint32_t uncompressedSize = 0;
			bool     compressed = false;
		};

		/// @brief Storage for a data block created (and owned) by AddDataBlock(), backed by an in-memory buffer
		class OwnedDataBlock
		{
		public:
			OwnedDataBlock(bool compressed)
			: _compressionStream(_memoryStream)
			, _compressed(compressed)
			{
			}

			Stream& GetStream() { return _compressed ? static_cast<Stream&>(_compressionStream) : static_cast<Stream&>(_memoryStream); }

		private:
			MemoryStream     _memoryStream;
			CompressionStream _compressionStream;
			bool             _compressed;
		};

	private:
		UID      _uid;
		uint64_t _assetType = 0;
		uint64_t _contentHash = 0;

		Vector<SourceInfo> _sources;

		Document _importSettings;

		Vector<DataBlockInfo>     _dataBlocks;
		Vector<FileStream>        _internalDataBlockStream;
		Vector<CompressionStream> _internalCompressionStream;
		Vector<OwnedDataBlock*>   _ownedDataBlocks;
	};
}
