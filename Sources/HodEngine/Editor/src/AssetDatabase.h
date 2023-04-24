#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>

#include <HodEngine/Core/Src/UID.h>
#include <HodEngine/Core/Src/Event.h>
#include <HodEngine/Core/Src/LinkedList.h>
#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>
#include <HodEngine/Core/Src/Singleton.h>

namespace hod::editor
{
	class Asset;
	class Project;

	/// @brief 
	class AssetDatabase
	{
		_Singleton(AssetDatabase)

	public:

		struct FileSystemMapping
		{
			enum class Type
			{
				AssetType,
				FolderType,
			};

			std::filesystem::file_time_type	_lastWriteTime;
			std::filesystem::path			_path;
			Type							_type;
			UID								_uid;

			std::shared_ptr<Asset>			_asset = nullptr;

			std::vector<FileSystemMapping*>	_childrenAsset;
			std::vector<FileSystemMapping*>	_childrenFolder;

			//const FileSystemMapping*		_firstChildrenAsset = nullptr;
			//const FileSystemMapping*		_nextChildrenAsset = nullptr;

			//const FileSystemMapping*		_firstChildrenFolder = nullptr;
			//const FileSystemMapping*		_nextChildrenFolder = nullptr;

			//LinkedList<FileSystemMapping>	_childrenAsset;
			//LinkedList<FileSystemMapping>	_childrenFolder;

			FileSystemMapping*				_parentFolder = nullptr;
		};

	public:
											~AssetDatabase();

		bool								Init();

		const FileSystemMapping&			GetAssetRootNode() const;
		FileSystemMapping*					FindFileSystemMappingFromPath(const std::filesystem::path& path) const;

		std::filesystem::path				CreateFolder(const std::filesystem::path& path);

		template<typename Type>
		std::filesystem::path				CreateAsset(const std::filesystem::path& path);

		void								Rename(const FileSystemMapping& node, const std::string& newName);
		void								Delete(const FileSystemMapping& node);

		bool								Import(const std::filesystem::path& path);

	private:

		void								OnProjectLoaded(Project* project);
		void								OnProjectClosed(Project* project);

		void								ExploreAndDetectAsset(FileSystemMapping* parentFileSystemMapping);
		void								FilesystemWatcherJob();

		const std::filesystem::path&		UIDToAssetPath(const UID& uid) const;
		const UID&							AssetPathToUID(const std::filesystem::path& path) const;

		

	private:

		static std::filesystem::path		GenerateUniqueAssetPath(const std::filesystem::path& path);

	private:

		std::map<UID, std::shared_ptr<Asset>> _uidToAssetMap;
		FileSystemMapping					_rootFileSystemMapping;

		void*								_filesystemWatcherHandle;
		MemberFunctionJob<AssetDatabase>	_filesystemWatcherJob;
	};
}

#include "AssetDatabase.inl"
