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
			String   _name;
			uint32_t _offset = 0; // Relative to the start of the Data section
			uint32_t _size = 0;
		};

	public:
		static constexpr uint32_t FORMAT_VERSION = 1;

	public:
		AssetContainer() = default;
		~AssetContainer() = default;

		/// @brief Read only the fixed Header and the SourcePath block (no Content/Data), for fast scans at startup
		bool LoadHeader(const Path& path);

		/// @brief Read the whole asset (Header + SourcePath + Content + Data)
		bool Load(const Path& path);

		/// @brief Write the whole asset (Header + SourcePath + Content + Data)
		bool Save(const Path& path);

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

		DocumentNode&       GetContentRoot();
		const DocumentNode& GetContentRoot() const;

		const Vector<DataBlockInfo>& GetDataBlocks() const;
		const DataBlockInfo*         FindDataBlock(std::string_view name) const;
		const uint8_t*               GetDataBlockBuffer(const DataBlockInfo& dataBlock) const;

		void SetDataBlock(std::string_view name, const void* buffer, uint32_t size);
		void RemoveDataBlock(std::string_view name);

	private:
		bool ReadHeader(Stream& stream);

	private:
		static const uint8_t MAGIC[8];

	private:
		UID      _uid;
		uint64_t _assetType = 0;
		uint64_t _contentHash = 0;

		uint64_t _sourceHash = 0;
		Path     _sourcePath;

		Document _importSettings;
		Document _content; // Root holds "_content" and the reserved "_dataBlocks" array

		Vector<DataBlockInfo> _dataBlocks;
		Vector<uint8_t>       _data;
	};
}
