#include "HodEngine/Editor/AssetDatabase.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"

#include <HodEngine/Core/Frame/FrameSequencer.hpp>
#include <HodEngine/Core/Output.hpp>
#include <HodEngine/Core/StringConversion.hpp>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#include <WinBase.h>
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

		::CloseHandle(_hDir);
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
		std::wstring assetFolderPath;
		StringConversion::StringToWString(project->GetAssetDirPath().string(), assetFolderPath);
		_hDir = CreateFileW(assetFolderPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  		_overlapped.hEvent = CreateEventW(NULL, FALSE, 0, NULL);

		::ReadDirectoryChangesW(_hDir, _changeBuf, sizeof(_changeBuf), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &_overlapped, NULL);

		/*/
		_filesystemWatcherHandle = ::FindFirstChangeNotificationW(assetFolderPath.c_str(), TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_LAST_WRITE);
		*/
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
		DWORD result = WaitForSingleObject(_overlapped.hEvent, 0);

		if (result == WAIT_OBJECT_0)
		{
			DWORD bytes_transferred;
      		GetOverlappedResult(_hDir, &_overlapped, &bytes_transferred, FALSE);

			std::filesystem::path oldFilePathToRename;

			FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)_changeBuf;
			while (true)
			{
				std::wstring result(fni->FileName, fni->FileNameLength / sizeof(wchar_t));
				std::string relativefilePath;
				StringConversion::WStringToString(result, relativefilePath);

				std::filesystem::path filePath = Project::GetInstance()->GetAssetDirPath().string();
				filePath /= relativefilePath;

				switch (fni->Action)
				{
					case FILE_ACTION_ADDED:
					{
						FileSystemMapping* node = new FileSystemMapping();
						node->_path = filePath;
						node->_parentFolder = FindFileSystemMappingFromPath(filePath.parent_path());

						if (std::filesystem::is_directory(filePath))
						{
							node->_parentFolder->_childrenFolder.push_back(node);
							node->_type = FileSystemMapping::Type::FolderType;
						}
						else
						{
							node->_parentFolder->_childrenAsset.push_back(node);
							node->_type = FileSystemMapping::Type::AssetType;
							// todo create Asset?
							Import(filePath);
						}
					}
					break;

					case FILE_ACTION_REMOVED:
					{
						FileSystemMapping* nodeToMove = FindFileSystemMappingFromPath(filePath);
						if (nodeToMove != nullptr)
						{
							DeleteNode(*nodeToMove);
						}
					}
					break;

					case FILE_ACTION_MODIFIED:
					{
						if (std::filesystem::is_directory(filePath) == false)
						{
							Import(filePath);
						}
					}
					break;

					case FILE_ACTION_RENAMED_OLD_NAME:
					{
						oldFilePathToRename = filePath;
					}
					break;

					case FILE_ACTION_RENAMED_NEW_NAME:
					{
						FileSystemMapping* nodeToMove = FindFileSystemMappingFromPath(oldFilePathToRename);
						if (nodeToMove != nullptr)
						{
							MoveNode(*nodeToMove, filePath);
						}
					}
					break;
				}

				if (fni->NextEntryOffset == 0)
				{
					break;
				}
				fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>((uint8_t*)fni + fni->NextEntryOffset);
			}

			::ReadDirectoryChangesW(_hDir, _changeBuf, sizeof(_changeBuf), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &_overlapped, NULL);
		}

		//if (::WaitForSingleObject(_filesystemWatcherHandle, 0) == WAIT_OBJECT_0)

		//::FindNextChangeNotification(_filesystemWatcherHandle);
		// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstchangenotificationa
		// https://docs.microsoft.com/en-us/windows/win32/fileio/obtaining-directory-change-notifications
		// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
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

			bool found = false;
			for (AssetDatabase::FileSystemMapping* childNode : currentNode->_childrenFolder)
			{
				if (splitPath == childNode->_path)
				{
					found = true;
					currentNode = childNode;
					break;
				}
			}

			if (found == false && deep == 0)
			{
				for (AssetDatabase::FileSystemMapping* childNode : currentNode->_childrenAsset)
				{
					if (splitPath == childNode->_path)
					{
						return childNode;
					}
				}
				return nullptr;
			}
		}

		return currentNode;
	}

	/// @brief 
	/// @param folder 
	/// @return 
	AssetDatabase::FileSystemMapping& AssetDatabase::GetAssetRootNode()
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
	void AssetDatabase::Move(FileSystemMapping& node, const std::filesystem::path& newPath)
	{
		if (node._path == newPath)
		{
			return;
		}

		std::filesystem::path finalPath = GenerateUniqueAssetPath(newPath);

		if (node._type == FileSystemMapping::Type::FolderType)
		{
			std::filesystem::rename(node._path, finalPath);
		}
		else
		{
			std::filesystem::rename(node._path, finalPath);
			std::filesystem::path metaPath = node._path;
			metaPath.concat(".meta");
			std::filesystem::path newMetaPath = finalPath;
			newMetaPath.concat(".meta");
			std::filesystem::rename(metaPath, newMetaPath);
		}

		MoveNode(node, finalPath);
	}

	/// @brief 
	void AssetDatabase::FileSystemMapping::RefreshPathFromParent()
	{
		_path = _parentFolder->_path / _path.filename();

		for (AssetDatabase::FileSystemMapping* child : _childrenFolder)
		{
			child->RefreshPathFromParent();
		}

		for (AssetDatabase::FileSystemMapping* child : _childrenAsset)
		{
			child->RefreshPathFromParent();
		}
	}

	/// @brief 
	/// @param node 
	void AssetDatabase::Delete(FileSystemMapping& node)
	{
		std::filesystem::remove_all(node._path);
		std::filesystem::remove_all(node._path.concat(".meta"));

		DeleteNode(node);
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

	/// @brief 
	/// @param node 
	/// @param newPath 
	void AssetDatabase::MoveNode(FileSystemMapping& node, const std::filesystem::path& newPath)
	{
		node._path = newPath;

		AssetDatabase::FileSystemMapping* parentNode = node._parentFolder;
		if (parentNode == nullptr)
		{
			return;
		}

		AssetDatabase::FileSystemMapping* newParentNode = FindFileSystemMappingFromPath(newPath.parent_path());
		if (newParentNode == nullptr)
		{
			return;
		}

		if (newParentNode != parentNode)
		{
			auto it = std::find(node._parentFolder->_childrenFolder.begin(), node._parentFolder->_childrenFolder.end(), &node);
			if (it != node._parentFolder->_childrenFolder.end())
			{
				node._parentFolder->_childrenFolder.erase(it);
			}

			it = std::find(node._parentFolder->_childrenAsset.begin(), node._parentFolder->_childrenAsset.end(), &node);
			if (it != node._parentFolder->_childrenAsset.end())
			{
				node._parentFolder->_childrenAsset.erase(it);
			}

			node._parentFolder = newParentNode;
			if (node._type == FileSystemMapping::Type::FolderType)
			{
				newParentNode->_childrenFolder.push_back(&node);
			}
			else
			{
				newParentNode->_childrenAsset.push_back(&node);
			}
		}

		if (node._type == FileSystemMapping::Type::FolderType)
		{
			node.RefreshPathFromParent();
		}
		else
		{
			if (node._asset != nullptr)
			{
				node._asset->SetPath(newPath);
			}
		}
	}

	/// @brief 
	/// @param node 
	void AssetDatabase::DeleteNode(FileSystemMapping& node)
	{
		if (node._type == AssetDatabase::FileSystemMapping::Type::AssetType)
		{
			std::vector<FileSystemMapping*>::const_iterator it = std::find(node._parentFolder->_childrenAsset.begin(), node._parentFolder->_childrenAsset.end(), &node);
			node._parentFolder->_childrenAsset.erase(it);
		}

		if (node._type == AssetDatabase::FileSystemMapping::Type::FolderType)
		{
			std::vector<FileSystemMapping*>::const_iterator it = std::find(node._parentFolder->_childrenFolder.begin(), node._parentFolder->_childrenFolder.end(), &node);
			node._parentFolder->_childrenFolder.erase(it);
		}

		delete &node;
	}
}
