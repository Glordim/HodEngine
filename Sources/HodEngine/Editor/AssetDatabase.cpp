#include "HodEngine/Editor/AssetDatabase.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"

#include <HodEngine/Core/Frame/FrameSequencer.hpp>
#include <HodEngine/Core/Output.hpp>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#endif
#include <vector>

namespace hod::editor
{
	/// @brief 
	/// @param path 
	/// @return 
	std::filesystem::path AssetDatabase::GenerateUniqueAssetPath(const std::filesystem::path& path)
	{
		uint32_t count = 1;
		std::filesystem::path uniquePath(path);

		while (std::filesystem::exists(uniquePath) == true)
		{
			uniquePath = path;
			uniquePath.replace_filename(path.stem().string() + " " + std::to_string(count) + path.extension().string());
			++count;
		}

		return uniquePath;
	}

	_SingletonConstructor(AssetDatabase)
		: _filesystemWatcherJob(this, &AssetDatabase::FilesystemWatcherJob, JobQueue::FramedLowPriority)
	{
		_importers.push_back(&_defaultImporter);
		
		RegisterImporter<TextureImporter>();
	}

	/// @brief 
	AssetDatabase::~AssetDatabase()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_filesystemWatcherJob, FrameSequencer::Step::PreRender);

#if defined(PLATFORM_WINDOWS)
		::FindCloseChangeNotification(_filesystemWatcherHandle);
		_filesystemWatcherHandle = INVALID_HANDLE_VALUE;
#endif

		for (auto pair : _uidToAssetMap)
		{
			//delete pair.second;
		}

		_uidToAssetMap.clear();
	}

	/// @brief 
	/// @return 
	bool AssetDatabase::Init()
	{
		Project* project = Project::GetInstance();

#if defined(PLATFORM_WINDOWS)
		_filesystemWatcherHandle = ::FindFirstChangeNotification(project->GetAssetDirPath().string().c_str(), TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_SECURITY);
#endif

		_rootFileSystemMapping._asset = nullptr;
		_rootFileSystemMapping._type = FileSystemMapping::Type::FolderType;
		_rootFileSystemMapping._path = project->GetAssetDirPath();
		ExploreAndDetectAsset(&_rootFileSystemMapping);

		FrameSequencer::GetInstance()->InsertJob(&_filesystemWatcherJob, FrameSequencer::Step::PreRender);

		return true;
	}

	/// @brief 
	void AssetDatabase::FilesystemWatcherJob()
	{
#if defined(PLATFORM_WINDOWS)
		if (WaitForSingleObject(_filesystemWatcherHandle, 0) == WAIT_OBJECT_0)
		{
			FindNextChangeNotification(_filesystemWatcherHandle);
			// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstchangenotificationa
			// https://docs.microsoft.com/en-us/windows/win32/fileio/obtaining-directory-change-notifications
			// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
		}
#endif
	}

	/// @brief 
	/// @param dir 
	void AssetDatabase::ExploreAndDetectAsset(FileSystemMapping* fileSystemMapping)
	{
		std::filesystem::directory_iterator entries(fileSystemMapping->_path);

		for (const std::filesystem::directory_entry& entry : entries)
		{
			FileSystemMapping* childFileSystemMapping = new FileSystemMapping;
			childFileSystemMapping->_path = entry.path();
			childFileSystemMapping->_lastWriteTime = entry.last_write_time();
			childFileSystemMapping->_parentFolder = fileSystemMapping;

			if (entry.is_directory() == true)
			{
				childFileSystemMapping->_asset = nullptr;
				childFileSystemMapping->_type = FileSystemMapping::Type::FolderType;
				ExploreAndDetectAsset(childFileSystemMapping);

				//fileSystemMapping->_childrenFolder.PushBack(&childFileSystemMapping->_childrenFolder);
				fileSystemMapping->_childrenFolder.push_back(childFileSystemMapping);
			}
			else
			{
				if (childFileSystemMapping->_path.extension().string() != ".meta")
				{
					std::shared_ptr<Asset> asset = std::make_shared<Asset>(childFileSystemMapping->_path);
					if (asset->Load() == true)
					{
						childFileSystemMapping->_asset = asset;
						childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;
						_uidToAssetMap.emplace(asset->GetUid(), asset);

						//fileSystemMapping->_childrenAsset.PushBack(&childFileSystemMapping->_childrenAsset);
						fileSystemMapping->_childrenAsset.push_back(childFileSystemMapping);
					}
					else
					{
						OUTPUT_ERROR("Unable to load Asset : %s", childFileSystemMapping->_path.string().c_str());
					}
				}
			}
		}
	}

	/// @brief 
	/// @param path 
	/// @return 
	AssetDatabase::FileSystemMapping* AssetDatabase::FindFileSystemMappingFromPath(const std::filesystem::path& path) const
	{
		int deep = 0;
		std::filesystem::path splitPath = path;
		while (splitPath != _rootFileSystemMapping._path)
		{
			if (splitPath.has_parent_path() == false)
			{
				return nullptr;
			}
			splitPath = splitPath.parent_path();
			++deep;
		}

		AssetDatabase::FileSystemMapping* currentNode = (AssetDatabase::FileSystemMapping*) & _rootFileSystemMapping;
		while (currentNode->_path != path)
		{
			splitPath = path;
			for (int i = 0; i < deep - 1; ++i)
			{
				splitPath = splitPath.parent_path();
			}
			--deep;

			bool founded = false;
			for (AssetDatabase::FileSystemMapping* childNode : currentNode->_childrenFolder)
			{
				if (splitPath == childNode->_path)
				{
					founded = true;
					currentNode = childNode;
					break;
				}
			}

			if (founded == false && deep == 0)
			{
				for (AssetDatabase::FileSystemMapping* childNode : currentNode->_childrenAsset)
				{
					if (splitPath == childNode->_path)
					{
						return childNode;
					}
				}
			}
		}

		return currentNode;
	}

	/// @brief 
	/// @param folder 
	/// @return 
	const AssetDatabase::FileSystemMapping& AssetDatabase::GetAssetRootNode() const
	{
		return _rootFileSystemMapping;
	}

	/// @brief 
	/// @param path 
	std::filesystem::path AssetDatabase::CreateFolder(const std::filesystem::path& path)
	{
		std::filesystem::path finalPath = GenerateUniqueAssetPath(path);

		if (std::filesystem::create_directory(finalPath) == true)
		{
			AssetDatabase::FileSystemMapping* parentNode = (AssetDatabase::FileSystemMapping*)FindFileSystemMappingFromPath(finalPath.parent_path());
			if (parentNode != nullptr)
			{
				FileSystemMapping* newFolderFileSystemMapping = new FileSystemMapping;
				newFolderFileSystemMapping->_path = finalPath;
				//newFolderFileSystemMapping->_lastWriteTime = entry.last_write_time();
				newFolderFileSystemMapping->_parentFolder = parentNode;
				newFolderFileSystemMapping->_asset = nullptr;
				newFolderFileSystemMapping->_type = FileSystemMapping::Type::FolderType;
				parentNode->_childrenFolder.push_back(newFolderFileSystemMapping);
				//parentNode->_childrenFolder.PushBack(&newFolderFileSystemMapping->_childrenFolder);
			}
		}

		return finalPath;
	}

	/// @brief 
	/// @param asset 
	/// @param path 
	/// @return 
	std::filesystem::path AssetDatabase::CreateAsset(Object& object, const std::filesystem::path& path)
	{
		return CreateAsset(&object, object.GetReflectionDescriptorV(), path);
	}

	/// @brief 
	/// @param instance 
	/// @param reflectionDescriptor 
	/// @param path 
	/// @return 
	std::filesystem::path AssetDatabase::CreateAsset(void* instance, ReflectionDescriptor* reflectionDescriptor, const std::filesystem::path& path)
	{
		FileSystemMapping* childFileSystemMapping = new FileSystemMapping;
		childFileSystemMapping->_path = path;
		//childFileSystemMapping->_path.replace_extension(".asset");
		childFileSystemMapping->_path = GenerateUniqueAssetPath(childFileSystemMapping->_path);
		//childFileSystemMapping->_lastWriteTime = 0;
		childFileSystemMapping->_parentFolder = FindFileSystemMappingFromPath(path.parent_path());
		childFileSystemMapping->_uid = UID::GenerateUID();
		childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;
		childFileSystemMapping->_asset = std::make_shared<Asset>(childFileSystemMapping->_path);

		if (childFileSystemMapping->_asset->Save(instance, reflectionDescriptor) == false)
		{
			// TODO message better return value
			return childFileSystemMapping->_path;
		}

		_uidToAssetMap.emplace(childFileSystemMapping->_uid, childFileSystemMapping->_asset);

		childFileSystemMapping->_parentFolder->_childrenAsset.push_back(childFileSystemMapping);

		return childFileSystemMapping->_path;
	}

	/// @brief 
	/// @param node 
	/// @param newName 
	void AssetDatabase::Rename(const FileSystemMapping& node, const std::string& newName)
	{
		AssetDatabase::FileSystemMapping* realNode = (AssetDatabase::FileSystemMapping*)FindFileSystemMappingFromPath(node._path);
		if (realNode == nullptr)
		{
			return;
		}

		std::filesystem::path newPath = realNode->_path;
		newPath = newPath.replace_filename(newName);
		newPath = GenerateUniqueAssetPath(newPath);

		if (realNode->_type == FileSystemMapping::Type::FolderType)
		{
			std::filesystem::rename(realNode->_path, newPath);
			// todo update all child node path
		}
		else
		{
			if (realNode->_asset == nullptr)
			{
				return;
			}

			std::filesystem::rename(realNode->_path, newPath);
		}

		realNode->_path = newPath;
	}

	/// @brief 
	/// @param node 
	void AssetDatabase::Delete(const FileSystemMapping& node)
	{
		AssetDatabase::FileSystemMapping* realNode = (AssetDatabase::FileSystemMapping*)FindFileSystemMappingFromPath(node._path);
		if (realNode == nullptr)
		{
			return;
		}

		std::filesystem::remove_all(realNode->_path);
		std::filesystem::remove_all(realNode->_path.concat(".meta"));

		if (realNode->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
		{
			std::vector<FileSystemMapping*>::const_iterator it = std::find(realNode->_parentFolder->_childrenAsset.begin(), realNode->_parentFolder->_childrenAsset.end(), realNode);
			realNode->_parentFolder->_childrenAsset.erase(it);
		}

		if (realNode->_type == AssetDatabase::FileSystemMapping::Type::FolderType)
		{
			std::vector<FileSystemMapping*>::const_iterator it = std::find(realNode->_parentFolder->_childrenFolder.begin(), realNode->_parentFolder->_childrenFolder.end(), realNode);
			realNode->_parentFolder->_childrenFolder.erase(it);
		}

		delete realNode;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool AssetDatabase::Import(const std::filesystem::path& path)
	{
		for (Importer* importer : _importers)
		{
			if (importer->CanImport(path) == true)
			{
				return importer->Import(path);
			}
		}

		return _defaultImporter.Import(path);
/*
		OUTPUT_ERROR("Can't import \"%s\", not importer support it", path.string().c_str());
		return false;
*/
	}

	/// @brief 
	/// @param asset 
	/// @return 
	bool AssetDatabase::ReimportAssetIfNecessary(std::shared_ptr<Asset> asset)
	{
		std::filesystem::path resourceFilePath = Project::GetInstance()->GetResourceDirPath() / asset->GetUid().ToString();
		resourceFilePath += ".dat";

		std::ifstream resourceFile(resourceFilePath);
		if (resourceFile.is_open() == false) // TODO replace by filesystem::exist ?
		{
			return Import(asset->GetPath());
		}

		return false;
	}

	/// @brief 
	/// @param name 
	/// @return 
	Importer* AssetDatabase::GetImporter(const std::string_view& name) const
	{
		for (Importer* importer : _importers)
		{
			if (importer->GetTypeName() == name)
			{
				return importer;
			}
		}

		return nullptr;
	}

	/// @brief 
	/// @return 
	const DefaultImporter& AssetDatabase::GetDefaultImporter() const
	{
		return _defaultImporter;
	}

	/// @brief 
	/// @return 
	bool AssetDatabase::Save()
	{
		for (auto& assetPair : _uidToAssetMap)
		{
			if (assetPair.second->IsDirty() == true)
			{
				if (assetPair.second->Save() == false)
				{
					return false;
				}
			}
		}

		return true;
	}

	/// @brief 
	/// @param result 
	/// @param from 
	/// @param resourceDescriptor 
	void AssetDatabase::ListAsset(std::vector<FileSystemMapping*>& result, const FileSystemMapping& from, ReflectionDescriptor* resourceDescriptor)
	{
		for (FileSystemMapping* assetNode : from._childrenAsset)
		{
			Importer* importer = GetImporter(assetNode->_asset->GetMeta()._importerType);
			if (importer != nullptr && resourceDescriptor == importer->GetResourceDescriptor()) // TODO default import can't be listed
			{
				result.push_back(assetNode);
			}
		}

		for (FileSystemMapping* folderNode : from._childrenFolder)
		{
			ListAsset(result, *folderNode, resourceDescriptor);
		}
	}

	/// @brief 
	/// @param uid 
	/// @return 
	std::shared_ptr<Asset> AssetDatabase::Find(const UID& uid) const
	{
		auto it = _uidToAssetMap.find(uid);
		if (it != _uidToAssetMap.end())
		{
			return it->second;
		}
		return nullptr; 
	}
}
