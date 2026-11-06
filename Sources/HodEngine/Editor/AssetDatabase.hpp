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
	class AssetType;
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

		/// @brief The registered type of this hash (Asset::GetType()), nullptr if none
		const AssetType* FindAssetType(uint64_t assetType) const;

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

		/// @brief An asset file the editor just wrote, known right away rather than once the file watcher reports it
		/// (e.g. to reference it on the same frame). Its folder must be known already.
		std::shared_ptr<Asset> AddWrittenAsset(const Path& path);

		void Move(FileSystemMapping& node, const Path& newPath);
		void Delete(FileSystemMapping& node);

		/// @brief By the editor module adding the type (through EditorModuleRegistration, which owns it)
		/// @return false (with an error) if a type of the same hash or of the same extension is registered already (by one module only),
		/// or if its extension is invalid (see AssetType::GetExtensionV)
		bool RegisterAssetType(const AssetType& assetType);

		/// @brief Its asset type is expected registered first (warned otherwise)
		/// @return false (with an error) if one of the extensions has an importer already: nothing registered then
		template<typename _Importer_, typename... Args>
		bool RegisterImporter(Args... extensions);

		// By the editor modules when they unload (through EditorModuleRegistration): their code goes with them
		void UnregisterAssetType(const AssetType& assetType);
		template<typename _Importer_>
		void UnregisterImporter();

		bool ReimportAssetIfNecessary(std::shared_ptr<Asset> asset);

		/// @brief The assets of the types cooking into resourceDescriptor (AssetType::GetCookedResource()). expectedSubType, when
		/// non-zero, additionally restricts the result to assets whose cooked asset subType (row/item RttiType, see
		/// ReflectionTraitAssetSubType) matches.
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
		void ReleaseNode(FileSystemMapping& node);

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

		/// @brief The factory of the entries of an importer type: identifies them to unregister it
		template<typename _Importer_>
		static Importer* NewImporter();
		void             UnregisterImporter(Importer* (*factory)());

	private:
		/// @brief One entry per registered importer type, matched by supported file extension. AcquireImporter allocates a fresh
		/// instance via _factory, ReleaseImporter destroys it, so concurrent batch jobs never share an instance. The entries (and
		/// _assetTypes) are only changed at startup and when an editor module unloads, while no import/cook job runs, so no
		/// locking is needed.
		struct ImporterEntry
		{
			Importer* (*_factory)() = nullptr;
			Vector<String> _extensions;
		};
		bool HasImporter(std::string_view extension) const;

		std::map<UID, std::shared_ptr<Asset>> _uidToAssetMap;
		std::unordered_multimap<Path, std::shared_ptr<Asset>> _sourcePathToAssetMap;
		FileSystemMapping                     _rootFileSystemMapping;

		FileSystemWatcher _fileSystemWatcherAsset;
		FileSystemWatcher _fileSystemWatcherSource;

		Vector<PendingChange> _pendingAssetChanges;
		Vector<PendingChange> _pendingSourceChanges;

		Vector<ImporterEntry*>   _importerEntries;
		Vector<const AssetType*> _assetTypes; // Owned by the EditorModuleRegistration of their module

		MemberFunctionJob<AssetDatabase> _updateJob;
	};
}

#include "HodEngine/Editor/AssetDatabase.inl"
