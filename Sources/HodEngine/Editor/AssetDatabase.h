#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>

#include <HodEngine/Core/UID.h>
#include <HodEngine/Core/Event.h>
#include <HodEngine/Core/LinkedList.h>
#include <HodEngine/Core/Job/MemberFunctionJob.h>
#include <HodEngine/Core/Singleton.h>

namespace hod::editor
{
	class Asset;
	class Project;
	class Importer;

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
		std::filesystem::path				CreateAsset(std::shared_ptr<Asset> asset, const std::filesystem::path& path);

		void								Rename(const FileSystemMapping& node, const std::string& newName);
		void								Delete(const FileSystemMapping& node);

		bool								Import(const std::filesystem::path& path);

		template<typename _Importer_>
		bool								RegisterImporter();
		Importer*							GetImporter(const std::string_view& name) const;

		bool								ReimportAssetIfNecessary(std::shared_ptr<Asset> asset);

	private:

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

		std::vector<Importer*>				_importers;
	};
}

#include "HodEngine/Editor/AssetDatabase.inl"
