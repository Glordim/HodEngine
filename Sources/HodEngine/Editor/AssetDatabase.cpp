#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Project.hpp"


#include "HodEngine/GameSystems/Frame/FrameSequencer.hpp"
#include <HodEngine/Core/Output/OutputService.hpp>

#include "HodEngine/Core/Vector.hpp"

#include <cctype>
#include <filesystem> // todo remove
#include <memory>

namespace hod::inline editor
{
	/// @brief
	/// @param path
	/// @return
	Path AssetDatabase::GenerateUniqueAssetPath(const Path& path)
	{
		uint32_t count = 1;
		Path     uniquePath(path);

		while (FileSystem::GetInstance()->Exists(uniquePath) == true)
		{
			uniquePath = path;
			uniquePath.ReplaceFilename(path.Stem().GetString() + " " + std::to_string(count).c_str() + path.Extension().GetString());
			++count;
		}

		return uniquePath;
	}

	_SingletonConstructor(AssetDatabase)
	: _updateJob(this, &AssetDatabase::UpdateFileWatchers)
	{
	}

	/// @brief
	AssetDatabase::~AssetDatabase()
	{
		_fileSystemWatcherAsset.Cleanup();
		_fileSystemWatcherSource.Cleanup();

		ClearFilesystemMapping(_rootFileSystemMapping);

		for (auto pair : _uidToAssetMap)
		{
			// DefaultAllocator::GetInstance().Delete(pair.second);
		}

		_uidToAssetMap.clear();

		for (ImporterEntry* entry : _importerEntries)
		{
			DefaultAllocator::GetInstance().Delete(entry);
		}

		for (CookerEntry* entry : _cookerEntries)
		{
			DefaultAllocator::GetInstance().Delete(entry);
		}
	}

	/// @brief
	void AssetDatabase::ClearFilesystemMapping(FileSystemMapping& filesystemMapping)
	{
		for (FileSystemMapping* child : filesystemMapping._childrenFolder)
		{
			ClearFilesystemMapping(*child);
			DefaultAllocator::GetInstance().Delete(child);
		}

		for (FileSystemMapping* child : filesystemMapping._childrenAsset)
		{
			ClearFilesystemMapping(*child);
			DefaultAllocator::GetInstance().Delete(child);
		}
	}

	/// @brief
	/// @return
	bool AssetDatabase::Init()
	{
		Project* project = Project::GetInstance();

		_rootFileSystemMapping._asset = nullptr;
		_rootFileSystemMapping._type = FileSystemMapping::Type::FolderType;
		_rootFileSystemMapping._path = project->GetAssetDirPath();
		ExploreAndDetectAsset(&_rootFileSystemMapping);

		if (_fileSystemWatcherAsset.Init(project->GetAssetDirPath(), std::bind(&AssetDatabase::FileSystemWatcherAssetOnCreateFile, this, std::placeholders::_1),
		                                 std::bind(&AssetDatabase::FileSystemWatcherAssetOnDeleteFile, this, std::placeholders::_1),
		                                 std::bind(&AssetDatabase::FileSystemWatcherAssetOnChangeFile, this, std::placeholders::_1),
		                                 std::bind(&AssetDatabase::FileSystemWatcherAssetOnMoveFile, this, std::placeholders::_1, std::placeholders::_2)) == false)
		{
			return false;
		}

		if (_fileSystemWatcherSource.Init(project->GetSourceDirPath(), std::bind(&AssetDatabase::FileSystemWatcherSourceOnCreateFile, this, std::placeholders::_1),
		                                  std::bind(&AssetDatabase::FileSystemWatcherSourceOnDeleteFile, this, std::placeholders::_1),
		                                  std::bind(&AssetDatabase::FileSystemWatcherSourceOnChangeFile, this, std::placeholders::_1),
		                                  std::bind(&AssetDatabase::FileSystemWatcherSourceOnMoveFile, this, std::placeholders::_1, std::placeholders::_2)) == false)
		{
			return false;
		}

		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::PostRender);

		return true;
	}

	void AssetDatabase::UpdateFileWatchers()
	{
		_fileSystemWatcherAsset.Update();
		_fileSystemWatcherSource.Update();
	}

	/// @brief
	/// @param dir
	void AssetDatabase::ExploreAndDetectAsset(FileSystemMapping* fileSystemMapping)
	{
		std::filesystem::directory_iterator entries(fileSystemMapping->_path.CStr());

		for (const std::filesystem::directory_entry& entry : entries)
		{
			Path path = entry.path().string().c_str();
			if (path.HasExtension() && path.Extension().GetString() == ".meta")
			{
				continue;
			}

			FileSystemMapping* childFileSystemMapping = DefaultAllocator::GetInstance().New<FileSystemMapping>();
			childFileSystemMapping->_path = path;
			// childFileSystemMapping->_lastWriteTime = entry.last_write_time();
			childFileSystemMapping->_parentFolder = fileSystemMapping;

			if (entry.is_directory() == true)
			{
				childFileSystemMapping->_asset = nullptr;
				childFileSystemMapping->_type = FileSystemMapping::Type::FolderType;
				ExploreAndDetectAsset(childFileSystemMapping);

				// fileSystemMapping->_childrenFolder.PushBack(&childFileSystemMapping->_childrenFolder);
				fileSystemMapping->_childrenFolder.push_back(childFileSystemMapping);
			}
			else
			{
				std::shared_ptr<Asset> asset = std::make_shared<Asset>(childFileSystemMapping->_path);
				childFileSystemMapping->_asset = asset;
				childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;

				// fileSystemMapping->_childrenAsset.PushBack(&childFileSystemMapping->_childrenAsset);
				fileSystemMapping->_childrenAsset.push_back(childFileSystemMapping);

				if (asset->Load() == false)
				{
					OUTPUT_ERROR("Unable to load Asset : {}", childFileSystemMapping->_path);
				}
				else
				{
					if (asset->HasSource())
					{
						_sourcePathToAssetMap.emplace(asset->GetSourcePath(), asset);
					}

					_uidToAssetMap.emplace(asset->GetUid(), asset);
				}
			}
		}
	}

	/// @brief
	/// @param path
	/// @return
	AssetDatabase::FileSystemMapping* AssetDatabase::FindFileSystemMappingFromPath(const Path& path) const
	{
		int  deep = 0;
		Path splitPath = path;
		while (splitPath != _rootFileSystemMapping._path)
		{
			if (splitPath.HasParentPath() == false)
			{
				return nullptr;
			}
			splitPath = splitPath.ParentPath();
			++deep;
		}

		AssetDatabase::FileSystemMapping* currentNode = (AssetDatabase::FileSystemMapping*)&_rootFileSystemMapping;
		while (currentNode->_path != path)
		{
			splitPath = path;
			for (int i = 0; i < deep - 1; ++i)
			{
				splitPath = splitPath.ParentPath();
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
	Path AssetDatabase::CreateFolder(const Path& path)
	{
		Path finalPath = GenerateUniqueAssetPath(path);

		if (FileSystem::GetInstance()->CreateDirectories(finalPath) == true)
		{
			AssetDatabase::FileSystemMapping* parentNode = (AssetDatabase::FileSystemMapping*)FindFileSystemMappingFromPath(finalPath.ParentPath());
			if (parentNode != nullptr)
			{
				FileSystemMapping* newFolderFileSystemMapping = DefaultAllocator::GetInstance().New<FileSystemMapping>();
				newFolderFileSystemMapping->_path = finalPath;
				// newFolderFileSystemMapping->_lastWriteTime = entry.last_write_time();
				newFolderFileSystemMapping->_parentFolder = parentNode;
				newFolderFileSystemMapping->_asset = nullptr;
				newFolderFileSystemMapping->_type = FileSystemMapping::Type::FolderType;
				parentNode->_childrenFolder.push_back(newFolderFileSystemMapping);
				// parentNode->_childrenFolder.PushBack(&newFolderFileSystemMapping->_childrenFolder);
			}
		}

		return finalPath;
	}

	/// @brief
	/// @param instance
	/// @param reflectionDescriptor
	/// @param path
	/// @return
	Path AssetDatabase::CreateAsset(void* instance, ReflectionDescriptor* reflectionDescriptor, std::shared_ptr<ImporterSettings> importerSettings, const char* importerType,
	                                const Path& path)
	{
		FileSystemMapping* childFileSystemMapping = DefaultAllocator::GetInstance().New<FileSystemMapping>();
		childFileSystemMapping->_path = path;
		// childFileSystemMapping->_path.replace_extension(".asset");
		childFileSystemMapping->_path = GenerateUniqueAssetPath(childFileSystemMapping->_path);
		// childFileSystemMapping->_lastWriteTime = 0;
		childFileSystemMapping->_parentFolder = FindFileSystemMappingFromPath(path.ParentPath());
		childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;
		childFileSystemMapping->_asset = std::make_shared<Asset>(childFileSystemMapping->_path);
		if (importerSettings != nullptr && importerType != nullptr)
		{
			//childFileSystemMapping->_asset->GetMeta().SetImporterConfig(importerSettings, importerType); // TODO
		}

		if (childFileSystemMapping->_asset->Save(instance, reflectionDescriptor) == false)
		{
			// TODO message better return value
			DefaultAllocator::GetInstance().Delete(childFileSystemMapping);
			return "";
		}

		_uidToAssetMap.emplace(childFileSystemMapping->_asset->GetUid(), childFileSystemMapping->_asset);

		childFileSystemMapping->_parentFolder->_childrenAsset.push_back(childFileSystemMapping);

		return childFileSystemMapping->_path;
	}

	/// @brief
	/// @param node
	/// @param newName
	void AssetDatabase::Move(FileSystemMapping& node, const Path& newPath)
	{
		if (node._path == newPath)
		{
			return;
		}

		Path finalPath = GenerateUniqueAssetPath(newPath);

		if (node._type == FileSystemMapping::Type::FolderType)
		{
			FileSystem::GetInstance()->Rename(node._path, finalPath);
		}
		else
		{
			FileSystem::GetInstance()->Rename(node._path, finalPath);
			Path metaPath = node._path;
			metaPath.Concat(".meta");
			Path newMetaPath = finalPath;
			newMetaPath.Concat(".meta");
			FileSystem::GetInstance()->Rename(metaPath, newMetaPath);
		}

		MoveNode(node, finalPath);
	}

	/// @brief
	void AssetDatabase::FileSystemMapping::RefreshPathFromParent()
	{
		_path = _parentFolder->_path / _path.Filename();

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
		FileSystem::GetInstance()->RemoveAll(node._path);
		FileSystem::GetInstance()->RemoveAll(node._path.Concat(".meta"));

		DeleteNode(node);
	}

	namespace
	{
		String ToLowerCopy(std::string_view value)
		{
			String result(value);
			for (char& c : result)
			{
				c = static_cast<char>(std::tolower(static_cast<int>(c)));
			}
			return result;
		}
	}

	Cooker* AssetDatabase::AcquireCooker(uint64_t type)
	{
		for (CookerEntry* entry : _cookerEntries)
		{
			if (entry->_assetType == type)
			{
				return entry->_factory();
			}
		}
		return nullptr;
	}

	void AssetDatabase::ReleaseCooker(Cooker* cooker)
	{
		DefaultAllocator::GetInstance().Delete(cooker);
	}

	Importer* AssetDatabase::AcquireImporter(std::string_view extension)
	{
		String extensionLower = ToLowerCopy(extension);
		for (ImporterEntry* entry : _importerEntries)
		{
			for (const String& supportedExtension : entry->_extensions)
			{
				if (extensionLower == supportedExtension)
				{
					return entry->_factory();
				}
			}
		}
		return nullptr;
	}

	void AssetDatabase::ReleaseImporter(Importer* importer)
	{
		DefaultAllocator::GetInstance().Delete(importer);
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
	void AssetDatabase::ListAsset(Vector<FileSystemMapping*>& result, const FileSystemMapping& from, ReflectionDescriptor* resourceDescriptor)
	{
		for (FileSystemMapping* assetNode : from._childrenAsset)
		{
			if (resourceDescriptor->GetType() == assetNode->_asset->GetType()) // TODO resourceDescriptorType is not AssetType
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
	void AssetDatabase::MoveNode(FileSystemMapping& node, const Path& newPath)
	{
		node._path = newPath;

		AssetDatabase::FileSystemMapping* parentNode = node._parentFolder;
		if (parentNode == nullptr)
		{
			return;
		}

		AssetDatabase::FileSystemMapping* newParentNode = FindFileSystemMappingFromPath(newPath.ParentPath());
		if (newParentNode == nullptr)
		{
			return;
		}

		if (newParentNode != parentNode)
		{
			auto it = std::find(node._parentFolder->_childrenFolder.Begin(), node._parentFolder->_childrenFolder.End(), &node);
			if (it != node._parentFolder->_childrenFolder.End())
			{
				node._parentFolder->_childrenFolder.Erase(it);
			}

			it = std::find(node._parentFolder->_childrenAsset.Begin(), node._parentFolder->_childrenAsset.End(), &node);
			if (it != node._parentFolder->_childrenAsset.End())
			{
				node._parentFolder->_childrenAsset.Erase(it);
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
			Vector<FileSystemMapping*>::ConstIterator it = std::find(node._parentFolder->_childrenAsset.Begin(), node._parentFolder->_childrenAsset.End(), &node);
			node._parentFolder->_childrenAsset.Erase(it);
		}

		if (node._type == AssetDatabase::FileSystemMapping::Type::FolderType)
		{
			Vector<FileSystemMapping*>::ConstIterator it = std::find(node._parentFolder->_childrenFolder.Begin(), node._parentFolder->_childrenFolder.End(), &node);
			node._parentFolder->_childrenFolder.Erase(it);
		}

		DefaultAllocator::GetInstance().Delete(&node);
	}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherAssetOnCreateFile(const Path& path)
	{
		if (FindFileSystemMappingFromPath(path) == nullptr)
		{
			if (FileSystem::GetInstance()->IsDirectory(path))
			{
				FileSystemMapping* node = DefaultAllocator::GetInstance().New<FileSystemMapping>();
				node->_path = path;
				node->_parentFolder = FindFileSystemMappingFromPath(path.ParentPath());

				node->_parentFolder->_childrenFolder.push_back(node);
				node->_type = FileSystemMapping::Type::FolderType;
			}
			else
			{
				std::shared_ptr<Asset> asset = std::make_shared<Asset>(path);
				if (asset->Load() == false)
				{
					OUTPUT_ERROR("Unable to load Asset : {}", path);
					return;
				}
				else
				{
					if (asset->HasSource())
					{
						_sourcePathToAssetMap.emplace(asset->GetSourcePath(), asset);
					}

					_uidToAssetMap.emplace(asset->GetUid(), asset);
				}

				FileSystemMapping* node = DefaultAllocator::GetInstance().New<FileSystemMapping>();
				node->_path = path;
				node->_parentFolder = FindFileSystemMappingFromPath(path.ParentPath());
				node->_parentFolder->_childrenAsset.push_back(node);
				node->_type = FileSystemMapping::Type::AssetType;
				node->_asset = asset;
			}
		}
	}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherAssetOnDeleteFile(const Path& path)
	{
		FileSystemMapping* nodeToMove = FindFileSystemMappingFromPath(path);
		if (nodeToMove != nullptr)
		{
			std::shared_ptr<Asset> asset = nodeToMove->_asset;
			if (asset != nullptr)
			{
				if (asset->HasSource())
				{
					auto it = _sourcePathToAssetMap.find(asset->GetSourcePath());
					if (it != _sourcePathToAssetMap.end())
					{
						_sourcePathToAssetMap.erase(it);
					}
				}

				auto it = _uidToAssetMap.find(asset->GetUid());
				if (it != _uidToAssetMap.end())
				{
					_uidToAssetMap.erase(it);
				}
			}

			DeleteNode(*nodeToMove);
		}
	}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherAssetOnChangeFile(const Path& path)
	{
		if (FileSystem::GetInstance()->IsDirectory(path) == false)
		{
			FileSystemMapping* nodeToReimport = FindFileSystemMappingFromPath(path);
			if (nodeToReimport != nullptr)
			{
				std::shared_ptr<Asset> asset = nodeToReimport->_asset;
				if (asset != nullptr)
				{
					Path oldSourcePath = asset->GetSourcePath();

					if (asset->Load() == false)
					{
						FileSystemWatcherAssetOnDeleteFile(path);
						OUTPUT_ERROR("Unable to load Asset : {}", path);
						return;
					}

					if (oldSourcePath != asset->GetSourcePath())
					{
						if (oldSourcePath.Empty() == false)
						{
							auto it = _sourcePathToAssetMap.find(asset->GetSourcePath());
							if (it != _sourcePathToAssetMap.end())
							{
								_sourcePathToAssetMap.erase(it);
							}
						}
						if (asset->GetSourcePath().Empty() == false)
						{
							_sourcePathToAssetMap.emplace(asset->GetSourcePath(), asset);
						}
					}
					_uidToAssetMap[asset->GetUid()] = asset;
				}
			}
		}
	}

	/// @brief
	/// @param oldPath
	/// @param newPath
	void AssetDatabase::FileSystemWatcherAssetOnMoveFile(const Path& oldPath, const Path& newPath)
	{
		FileSystemWatcherAssetOnDeleteFile(oldPath);
		FileSystemWatcherAssetOnCreateFile(newPath);
		/*
		FileSystemMapping* nodeToMove = FindFileSystemMappingFromPath(oldPath);
		if (nodeToMove != nullptr)
		{
		    MoveNode(*nodeToMove, newPath);
		}
		*/
	}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherSourceOnCreateFile(const Path& path)
	{
		FileSystemWatcherSourceOnChangeFile(path);
	}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherSourceOnDeleteFile(const Path& /*path*/) {}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherSourceOnChangeFile(const Path& path)
	{
		auto it = _sourcePathToAssetMap.find(path);
		if (it != _sourcePathToAssetMap.end())
		{
			// TODO reimport
		}
	}

	/// @brief
	/// @param oldPath
	/// @param newPath
	void AssetDatabase::FileSystemWatcherSourceOnMoveFile(const Path& /*oldPath*/, const Path& newPath)
	{
		FileSystemWatcherSourceOnChangeFile(newPath);
	}
}
