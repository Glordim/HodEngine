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
#include <HodEngine/Core/Time/SystemTime.hpp>
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

		/// @brief Name an asset type was registered with (RegisterCooker), empty if unknown
		std::string_view GetAssetTypeName(uint64_t assetType) const;

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

		// By the editor modules when they unload: their factories are in their code
		void                   UnregisterCooker(std::string_view assetType);
		void                   UnregisterImporter(std::string_view extension); // the importer of this extension, with all its extensions
		void                   UnregisterResource(std::string_view assetType);

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

		/// @brief A path the file watcher reported something on. It is only applied once quiet and readable (see ProcessPendingChanges()):
		/// the watcher notifies from the creation of a file, while it may still be being written
		struct PendingChange
		{
			Path                  _path;
			SystemTime::TimeStamp _lastEventTime;
		};

		static void QueueChange(Vector<PendingChange>& pendingChanges, const Path& path);
		void        ProcessPendingChanges(Vector<PendingChange>& pendingChanges, void (AssetDatabase::*applyChange)(const Path&));

		bool MoveKnownNode(const Path& oldPath, const Path& newPath);
		void ApplyAssetChange(const Path& path);
		void ApplySourceChange(const Path& path);

		void FileSystemWatcherAssetOnCreateFile(const Path& path);
		void FileSystemWatcherAssetOnDeleteFile(const Path& path);
		void FileSystemWatcherAssetOnChangeFile(const Path& path);

		void FileSystemWatcherSourceOnDeleteFile(const Path& path);
		void FileSystemWatcherSourceOnChangeFile(const Path& path);

		void ClearFilesystemMapping(FileSystemMapping& filesystemMapping);

	private:
		/// @brief One entry per registered Cooker type. _assetType is cached at registration so matching a
		/// type doesn't require an instance. Acquire allocates a fresh instance via _factory, Release
		/// destroys it, so concurrent batch jobs on the same type never share an instance. _cookerEntries is
		/// only changed at startup (RegisterCooker) and when an editor module unloads, while no Cook job runs, so no locking is needed.
		struct CookerEntry
		{
			Cooker* (*_factory)() = nullptr;
			uint64_t _assetType = 0;
			String   _assetTypeName;
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

		Vector<PendingChange> _pendingAssetChanges;
		Vector<PendingChange> _pendingSourceChanges;

		Vector<ImporterEntry*> _importerEntries;
		Vector<CookerEntry*> _cookerEntries;
		Vector<ResourceEntry*> _resourceEntries;

		MemberFunctionJob<AssetDatabase> _updateJob;
	};
}

#include "HodEngine/Editor/AssetDatabase.inl"
