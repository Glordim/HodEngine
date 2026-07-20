#pragma once
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <map>
#include <unordered_map>

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>

#include <HodEngine/Core/Hash.hpp>
#include <memory>

namespace hod::inline editor
{
	class Asset;
	class Project;
	class Importer;
	class Cooker;
	class ImporterSettings;

	/// @brief
	class HOD_EDITOR_API AssetDatabase
	{
		_Singleton(AssetDatabase)

	public:
		struct HOD_EDITOR_API FileSystemMapping
		{
			enum class Type
			{
				AssetType,
				FolderType,
			};

			FileSystem::FileTime _lastWriteTime;
			Path                 _path;
			Type                 _type;

			std::shared_ptr<Asset> _asset = nullptr;

			Vector<FileSystemMapping*> _childrenAsset;
			Vector<FileSystemMapping*> _childrenFolder;

			// const FileSystemMapping*		_firstChildrenAsset = nullptr;
			// const FileSystemMapping*		_nextChildrenAsset = nullptr;

			// const FileSystemMapping*		_firstChildrenFolder = nullptr;
			// const FileSystemMapping*		_nextChildrenFolder = nullptr;

			FileSystemMapping* _parentFolder = nullptr;

			void RefreshPathFromParent();
		};

	public:
		~AssetDatabase();

		bool Init();

		std::shared_ptr<Asset> Find(const UID& uid) const;

		/// @brief Allocates a fresh Cooker instance for the given asset type, usable to Cook concurrently
		/// with other in-flight Cook calls on the same type. Must be paired with ReleaseCooker, which
		/// destroys it.
		Cooker* AcquireCooker(uint64_t type);
		void    ReleaseCooker(Cooker* cooker);

		/// @brief Same as AcquireCooker/ReleaseCooker, for Importer batch/parallel usage.
		Importer* AcquireImporter(std::string_view extension);
		void      ReleaseImporter(Importer* importer);

		FileSystemMapping& GetAssetRootNode();
		FileSystemMapping* FindFileSystemMappingFromPath(const Path& path) const;

		Path CreateFolder(const Path& path);
		Path CreateAsset(void* instance, ReflectionDescriptor* reflectionDescriptor, std::shared_ptr<ImporterSettings> importerSettings, const char* importerType,
		                 const Path& path);

		template<typename _Object_, typename _Importer_>
		Path CreateAsset(const Path& path);

		void Move(FileSystemMapping& node, const Path& newPath);
		void Delete(FileSystemMapping& node);

		template<typename _Cooker_>
		bool                   RegisterCooker(std::string_view assetType);

		template<typename _Importer_, typename... Args>
		bool                   RegisterImporter(Args... extensions);

		template<typename _Resource_>
		bool                   RegisterResource(std::string_view assetType);

		bool ReimportAssetIfNecessary(std::shared_ptr<Asset> asset);

		/// @brief expectedSubType, when non-zero, additionally restricts the result to assets whose cooked
		/// asset subType (row/item RttiType, see ReflectionTraitAssetSubType) matches.
		void ListAsset(Vector<FileSystemMapping*>& result, const FileSystemMapping& from, const ReflectionDescriptor& resourceDescriptor, uint64_t expectedSubType = 0);

		void UpdateFileWatchers();

		static Path GenerateUniqueAssetPath(const Path& path);

	private:
		void ExploreAndDetectAsset(FileSystemMapping* parentFileSystemMapping);
		void FilesystemWatcherJob();

		const Path& UIDToAssetPath(const UID& uid) const;
		const UID&  AssetPathToUID(const Path& path) const;

		void MoveNode(FileSystemMapping& node, const Path& newPath);
		void DeleteNode(FileSystemMapping& node);

		void FileSystemWatcherAssetOnCreateFile(const Path& path);
		void FileSystemWatcherAssetOnDeleteFile(const Path& path);
		void FileSystemWatcherAssetOnChangeFile(const Path& path);
		void FileSystemWatcherAssetOnMoveFile(const Path& oldPath, const Path& newPath);

		void FileSystemWatcherSourceOnCreateFile(const Path& path);
		void FileSystemWatcherSourceOnDeleteFile(const Path& path);
		void FileSystemWatcherSourceOnChangeFile(const Path& path);
		void FileSystemWatcherSourceOnMoveFile(const Path& oldPath, const Path& newPath);

		void ClearFilesystemMapping(FileSystemMapping& filesystemMapping);

	private:
		/// @brief One entry per registered Cooker type. _assetType is cached at registration so matching a
		/// type doesn't require an instance. Acquire allocates a fresh instance via _factory, Release
		/// destroys it, so concurrent batch jobs on the same type never share an instance. _cookerEntries is
		/// only appended to at startup (RegisterCooker), before any Cook job runs, so no locking is needed.
		struct CookerEntry
		{
			Cooker* (*_factory)() = nullptr;
			uint64_t _assetType = 0;
		};

		/// @brief Same as CookerEntry, for Importer; matching is by supported file extension instead of a
		/// single type value, so _extensions is a list instead of a scalar.
		struct ImporterEntry
		{
			Importer* (*_factory)() = nullptr;
			Vector<String> _extensions;
		};

		struct ResourceEntry
		{
			const ReflectionDescriptor* _resourceDescriptor = nullptr;
			uint64_t _assetType = 0;
		};

		std::map<UID, std::shared_ptr<Asset>> _uidToAssetMap;
		std::unordered_multimap<Path, std::shared_ptr<Asset>> _sourcePathToAssetMap;
		FileSystemMapping                     _rootFileSystemMapping;

		FileSystemWatcher _fileSystemWatcherAsset;
		FileSystemWatcher _fileSystemWatcherSource;

		Vector<ImporterEntry*> _importerEntries;
		Vector<CookerEntry*> _cookerEntries;
		Vector<ResourceEntry*> _resourceEntries;

		MemberFunctionJob<AssetDatabase> _updateJob;
	};
}

#include "HodEngine/Editor/AssetDatabase.inl"
