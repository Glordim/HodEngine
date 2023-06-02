#include "HodEngine/Editor/AssetDatabase.h"

#include "HodEngine/Core/Stream/FileStream.h"

#include "HodEngine/Editor/Editor.h"
#include "HodEngine/Editor/Project.h"
#include "HodEngine/Editor/Asset.h"
#include "HodEngine/Editor/Importer/Importer.h"
#include "HodEngine/Editor/Importer/DefaultImporter.h"
#include "HodEngine/Editor/Importer/TextureImporter.h"

#include <HodEngine/Core/Frame/FrameSequencer.h>
#include <HodEngine/Core/Output.h>

#include <Windows.h>
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
		RegisterImporter<DefaultImporter>();
		RegisterImporter<TextureImporter>();
	}

	/// @brief 
	AssetDatabase::~AssetDatabase()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_filesystemWatcherJob, FrameSequencer::Step::PreRender);

		FindCloseChangeNotification(_filesystemWatcherHandle);
		_filesystemWatcherHandle = INVALID_HANDLE_VALUE;

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

		_filesystemWatcherHandle = ::FindFirstChangeNotification(project->GetAssetDirPath().string().c_str(), TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_SECURITY);

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
		if (WaitForSingleObject(_filesystemWatcherHandle, 0) == WAIT_OBJECT_0)
		{
			FindNextChangeNotification(_filesystemWatcherHandle);
			// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstchangenotificationa
			// https://docs.microsoft.com/en-us/windows/win32/fileio/obtaining-directory-change-notifications
			// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
		}
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
				for (AssetDatabase::FileSystemMapping* childNode : currentNode->_childrenFolder)
				{
					if (splitPath != childNode->_path)
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
	std::filesystem::path AssetDatabase::CreateAsset(std::shared_ptr<Asset> asset, const std::filesystem::path& path)
	{
		FileSystemMapping* childFileSystemMapping = new FileSystemMapping;
		childFileSystemMapping->_path = path;
		childFileSystemMapping->_path.replace_extension(".asset");
		childFileSystemMapping->_path = GenerateUniqueAssetPath(childFileSystemMapping->_path);
		//childFileSystemMapping->_lastWriteTime = 0;
		childFileSystemMapping->_parentFolder = FindFileSystemMappingFromPath(path.parent_path());
		childFileSystemMapping->_uid = UID::GenerateUID();
		childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;
		childFileSystemMapping->_asset = asset;

		_uidToAssetMap.emplace(childFileSystemMapping->_uid, asset);

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

		OUTPUT_ERROR("Can't import \"%s\", not importer support it", path.string().c_str());
		return false;
	}

	/// @brief 
	/// @param asset 
	/// @return 
	bool AssetDatabase::ReimportAssetIfNecessary(std::shared_ptr<Asset> asset)
	{
		std::filesystem::path resourceFilePath = Project::GetInstance()->GetResourceDirPath() / asset->GetUid().ToString();
		resourceFilePath += ".resource";

		core::FileStream resourceFile(resourceFilePath, core::FileMode::Read);
		if (resourceFile.CanRead() == false)
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
}
