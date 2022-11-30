#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>

#include <HodEngine/Core/Src/UID.h>
#include <HodEngine/Core/Src/Event.h>

#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>

namespace hod::editor
{
	class Asset;
	class Project;

	/// @brief 
	class AssetDatabase
	{
	public:
											AssetDatabase();
											~AssetDatabase();

		bool								Init();

		std::vector<std::filesystem::path>	GetSubFolder(const std::filesystem::path folder) const;

	private:

		struct FileSystemMapping
		{
			enum Type
			{
				AssetType,
				FolderType,
			};

			std::filesystem::file_time_type	_lastWriteTime;
			std::filesystem::path			_path;
			Type							_type;
			UID								_uid;

			std::shared_ptr<Asset*>			_asset = nullptr;
			
			std::vector<FileSystemMapping*>	_children;
		};

	private:

		void								OnProjectLoaded(Project* project);
		void								OnProjectClosed(Project* project);

		void								ExploreAndDetectAsset(const std::filesystem::path dir, FileSystemMapping* parentFileSystemMapping);
		void								FilesystemWatcherJob();

		const std::filesystem::path&		UIDToAssetPath(const UID& uid) const;
		const UID&							AssetPathToUID(const std::filesystem::path& path) const;

		FileSystemMapping*					FindFileSystemMappingFromPath(const std::filesystem::path path) const;

	private:

		static std::filesystem::path		GenerateUniqueAssetPath(const std::filesystem::path& path);

	private:

		std::map<UID, Asset*>				_uidToAssetMap;
		std::vector<FileSystemMapping*>		_filesystemMapping;

		Event<Project*>::Slot				_onProjectLoadedSlot;
		Event<Project*>::Slot				_onProjectClosedSlot;

		void*								_filesystemWatcherHandle;
		MemberFunctionJob<AssetDatabase>	_filesystemWatcherJob;
	};
}
