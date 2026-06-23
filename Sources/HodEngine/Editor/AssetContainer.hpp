#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <string_view>

namespace hod::inline editor
{
	/// @brief Binary container for .asset files: Header(fixed) + SourcePath block + Content block (json) + Data section
	class HOD_EDITOR_API AssetContainer
	{
	public:
		struct DataBlockInfo
		{
			uint64_t _hashName = 0;
			Stream*  _stream = nullptr;
		};

	public:
		static constexpr uint32_t FORMAT_VERSION = 1;

	public:
		AssetContainer() = default;
		~AssetContainer() = default;

		/// @brief Read only the fixed Header and the SourcePath block (no Content/Data), for fast scans at startup
		bool LoadHeader(const Path& path);
		bool LoadSource(const Path& path);

		/// @brief Read the whole asset (Header + SourcePath + Content + Data)
		bool Load(const Path& path);

		/// @brief Write the whole asset (Header + SourcePath + Content + Data)
		bool Save(const Path& path, const Path& tmpDir);

		const UID& GetUid() const;
		void       SetUid(const UID& uid);

		uint64_t GetAssetType() const;
		void     SetAssetType(uint64_t assetType);

		uint64_t GetContentHash() const;

		bool        HasSourcePath() const;
		const Path& GetSourcePath() const;
		void        SetSourcePath(const Path& sourcePath);

		uint64_t GetSourceHash() const;
		void     SetSourceHash(uint64_t sourceHash);

		DocumentNode&       GetImportSettings();
		const DocumentNode& GetImportSettings() const;

		const Vector<DataBlockInfo>& GetDataBlocks() const;
		const DataBlockInfo*         FindDataBlock(std::string_view name) const;

		void SetDataBlock(std::string_view name, Stream& stream);
		void RemoveDataBlock(std::string_view name);

	private:
		bool ReadHeader(Stream& stream);
		bool ReadSource(Stream& stream);

	private:
		static const uint8_t MAGIC[8];

		struct DataBlockLocation
		{
			uint64_t hashName = 0;
			uint64_t position = 0;
		};

	private:
		UID      _uid;
		uint64_t _assetType = 0;
		uint64_t _contentHash = 0;

		uint64_t _sourceHash = 0;
		Path     _sourcePath;

		Document _importSettings;

		Vector<DataBlockInfo> _dataBlocks;
		Vector<FileStream> _internalDataBlockStream;
	};
}
