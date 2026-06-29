#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <map>

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>

#include "HodEngine/Editor/Importer/DefaultImporter.hpp"
#include <unordered_map>

namespace hod::inline editor
{
	class Asset;
	class Project;
	class Importer;
	class Cooker;

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
		bool Save();

		std::shared_ptr<Asset> Find(const UID& uid) const;

		Importer* FindCompatibleImporter(std::string_view extension) const;
		Cooker* FindCompatibleCooker(uint64_t type) const;

		FileSystemMapping& GetAssetRootNode();
		FileSystemMapping* FindFileSystemMappingFromPath(const Path& path) const;

		Path CreateFolder(const Path& path);
		Path CreateAsset(void* instance, ReflectionDescriptor* reflectionDescriptor, std::shared_ptr<ImporterSettings> importerSettings, const char* importerType,
		                 const Path& path);

		template<typename _Object_, typename _Importer_>
		Path CreateAsset(const Path& path);

		void Move(FileSystemMapping& node, const Path& newPath);
		void Delete(FileSystemMapping& node);

		bool Import(const Path& path);
		bool Import(std::shared_ptr<Asset> asset);

		template<typename _Cooker_>
		bool                   RegisterCooker();

		template<typename _Importer_>
		bool                   RegisterImporter();
		Importer*              GetImporter(const std::string_view& name) const;
		const DefaultImporter& GetDefaultImporter() const;

		bool ReimportAssetIfNecessary(std::shared_ptr<Asset> asset);

		void ListAsset(Vector<FileSystemMapping*>& result, const FileSystemMapping& from, ReflectionDescriptor* resourceDescriptor);

		void UpdateFileWatchers();

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
		static Path GenerateUniqueAssetPath(const Path& path);

	private:
		std::map<UID, std::shared_ptr<Asset>> _uidToAssetMap;
		std::unordered_map<Path, std::shared_ptr<Asset>> _sourcePathToAssetMap;
		FileSystemMapping                     _rootFileSystemMapping;

		FileSystemWatcher _fileSystemWatcherAsset;
		FileSystemWatcher _fileSystemWatcherSource;

		Vector<Importer*> _importers;
		DefaultImporter   _defaultImporter;
		Vector<Cooker*> _cookers;

		MemberFunctionJob<AssetDatabase> _updateJob;
	};
}

#include "HodEngine/Editor/AssetDatabase.inl"
