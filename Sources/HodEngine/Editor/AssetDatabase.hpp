#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/LinkedList.hpp>
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>
#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Editor/Importer/DefaultImporter.hpp"

namespace hod
{
	class Object;
}

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

			void							RefreshPathFromParent();
		};

	public:
											~AssetDatabase();

		bool								Init();
		bool								Save();

		std::shared_ptr<Asset>				Find(const UID& uid) const;

		FileSystemMapping&					GetAssetRootNode();
		FileSystemMapping*					FindFileSystemMappingFromPath(const std::filesystem::path& path) const;

		std::filesystem::path				CreateFolder(const std::filesystem::path& path);
		std::filesystem::path				CreateAsset(Object& object, const std::filesystem::path& path);
		std::filesystem::path				CreateAsset(void* instance, ReflectionDescriptor* reflectionDescriptor, const std::filesystem::path& path);

		template<typename _Object_>
		std::filesystem::path				CreateAsset(const std::filesystem::path& path);

		void								Move(FileSystemMapping& node, const std::filesystem::path& newPath);
		void								Delete(const FileSystemMapping& node);

		bool								Import(const std::filesystem::path& path);

		template<typename _Importer_>
		bool								RegisterImporter();
		Importer*							GetImporter(const std::string_view& name) const;
		const DefaultImporter&				GetDefaultImporter() const;

		bool								ReimportAssetIfNecessary(std::shared_ptr<Asset> asset);

		void								ListAsset(std::vector<FileSystemMapping*>& result, const FileSystemMapping& from, ReflectionDescriptor* resourceDescriptor);

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
		DefaultImporter						_defaultImporter;
	};
}

#include "HodEngine/Editor/AssetDatabase.inl"
