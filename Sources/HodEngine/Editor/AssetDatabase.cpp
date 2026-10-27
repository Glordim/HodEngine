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
#include <cstdint>
#include <cstdio>
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

		for (ResourceEntry* entry : _resourceEntries)
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

		// Whatever the event, the path is only queued: what to do is decided from what is on disk once it is quiet (ProcessPendingChanges())
		auto queueAssetChange = [this](const Path& path) { QueueChange(_pendingAssetChanges, path); };
		auto queueAssetMove = [this](const Path& oldPath, const Path& newPath)
		{
			if (MoveKnownNode(oldPath, newPath) == false)
			{
				QueueChange(_pendingAssetChanges, oldPath);
				QueueChange(_pendingAssetChanges, newPath);
			}
		};
		if (_fileSystemWatcherAsset.Init(project->GetAssetDirPath(), queueAssetChange, queueAssetChange, queueAssetChange, queueAssetMove) == false)
		{
			return false;
		}

		auto queueSourceChange = [this](const Path& path) { QueueChange(_pendingSourceChanges, path); };
		auto queueSourceMove = [this](const Path& oldPath, const Path& newPath)
		{
			QueueChange(_pendingSourceChanges, oldPath);
			QueueChange(_pendingSourceChanges, newPath);
		};
		if (_fileSystemWatcherSource.Init(project->GetSourceDirPath(), queueSourceChange, queueSourceChange, queueSourceChange, queueSourceMove) == false)
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

		ProcessPendingChanges(_pendingAssetChanges, &AssetDatabase::ApplyAssetChange);
		ProcessPendingChanges(_pendingSourceChanges, &AssetDatabase::ApplySourceChange);
	}

	/// @brief One entry per path: the burst of events of a write (created, then modified several times) is applied once
	/// @param pendingChanges
	/// @param path
	void AssetDatabase::QueueChange(Vector<PendingChange>& pendingChanges, const Path& path)
	{
		SystemTime::TimeStamp now = SystemTime::Now();
		for (PendingChange& pendingChange : pendingChanges)
		{
			if (pendingChange._path == path)
			{
				pendingChange._lastEventTime = now;
				return;
			}
		}
		pendingChanges.PushBack({path, now});
	}

	/// @brief Applies the changes quiet for long enough. A file still open by its writer (it can't be opened yet) is retried later:
	/// the watcher has no "write finished" event, a writer done with a file has closed it
	/// @param pendingChanges
	/// @param applyChange
	void AssetDatabase::ProcessPendingChanges(Vector<PendingChange>& pendingChanges, void (AssetDatabase::*applyChange)(const Path&))
	{
		constexpr double quietDelay = 0.25; // Seconds without event on a path before it is considered

		SystemTime::TimeStamp now = SystemTime::Now();
		uint32_t              index = 0;
		while (index < pendingChanges.Size())
		{
			PendingChange& pendingChange = pendingChanges[index];
			if (SystemTime::ElapsedTimeInSeconds(pendingChange._lastEventTime, now) < quietDelay)
			{
				++index;
				continue;
			}

			if (FileSystem::GetInstance()->Exists(pendingChange._path) && FileSystem::GetInstance()->IsDirectory(pendingChange._path) == false)
			{
				// Not FileSystem::Open(), it logs an error on failure (std::fopen() on Windows fails with a sharing violation the same way)
				std::FILE* file = std::fopen(pendingChange._path.GetString().CStr(), "rb");
				if (file == nullptr)
				{
					pendingChange._lastEventTime = now;
					++index;
					continue;
				}
				std::fclose(file);
			}

			Path path = pendingChange._path;
			pendingChanges.Erase(pendingChanges.Begin() + index);
			(this->*applyChange)(path); // Can queue other changes (parent folder), hence after the erase
		}
	}

	/// @brief
	/// @param path
	/// @param root
	/// @return true if path is root or inside it
	static bool IsSameOrInside(const Path& path, const Path& root)
	{
		Path current = path;
		while (current != root)
		{
			if (current.HasParentPath() == false)
			{
				return false;
			}
			current = current.ParentPath();
		}
		return true;
	}

	/// @brief A rename reported by the watcher, applied right away (nothing is written, there is nothing to wait for) as a real move:
	/// the Asset and the content of a folder are kept, only their paths change (recursively, see MoveNode())
	/// @param oldPath
	/// @param newPath
	/// @return false when it isn't a known node to move (e.g. a move done by the editor, already applied): left to the pending changes then
	bool AssetDatabase::MoveKnownNode(const Path& oldPath, const Path& newPath)
	{
		FileSystemMapping* node = FindFileSystemMappingFromPath(oldPath);
		if (node == nullptr || node == &_rootFileSystemMapping || FindFileSystemMappingFromPath(newPath) != nullptr ||
		    FindFileSystemMappingFromPath(newPath.ParentPath()) == nullptr)
		{
			return false;
		}

		MoveNode(*node, newPath);

		// What was waiting under the old path (a file of the folder still being written...) follows it
		const String& oldPathString = oldPath.GetString();
		for (PendingChange& pendingChange : _pendingAssetChanges)
		{
			if (IsSameOrInside(pendingChange._path, oldPath))
			{
				pendingChange._path = Path(newPath.GetString() + pendingChange._path.GetString().SubStr(oldPathString.Length()));
			}
		}
		return true;
	}

	/// @brief Brings the database in line with what is on disk at path: removed, added or reloaded
	/// @param path
	void AssetDatabase::ApplyAssetChange(const Path& path)
	{
		FileSystemMapping* node = FindFileSystemMappingFromPath(path);
		if (FileSystem::GetInstance()->Exists(path) == false)
		{
			if (node != nullptr)
			{
				FileSystemWatcherAssetOnDeleteFile(path);
			}
			return;
		}

		if (node != nullptr)
		{
			FileSystemWatcherAssetOnChangeFile(path);
			return;
		}

		// The changes of a folder content can be quiet before the one of the folder itself (its modification date keeps changing)
		Path parentPath = path.ParentPath();
		if (FindFileSystemMappingFromPath(parentPath) == nullptr)
		{
			if (parentPath == _rootFileSystemMapping._path || parentPath.HasParentPath() == false)
			{
				return; // Not under the asset directory
			}
			ApplyAssetChange(parentPath);
			if (FindFileSystemMappingFromPath(parentPath) == nullptr)
			{
				return;
			}
		}
		FileSystemWatcherAssetOnCreateFile(path);
	}

	/// @brief
	/// @param path
	void AssetDatabase::ApplySourceChange(const Path& path)
	{
		if (FileSystem::GetInstance()->Exists(path))
		{
			FileSystemWatcherSourceOnChangeFile(path);
		}
		else
		{
			FileSystemWatcherSourceOnDeleteFile(path);
		}
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

			if (entry.is_directory() == true)
			{
				FileSystemMapping* childFileSystemMapping = DefaultAllocator::GetInstance().New<FileSystemMapping>();
				childFileSystemMapping->_path = path;
				// childFileSystemMapping->_lastWriteTime = entry.last_write_time();
				childFileSystemMapping->_parentFolder = fileSystemMapping;
				childFileSystemMapping->_asset = nullptr;
				childFileSystemMapping->_type = FileSystemMapping::Type::FolderType;
				ExploreAndDetectAsset(childFileSystemMapping);

				// fileSystemMapping->_childrenFolder.PushBack(&childFileSystemMapping->_childrenFolder);
				fileSystemMapping->_childrenFolder.push_back(childFileSystemMapping);
			}
			else
			{
				std::shared_ptr<Asset> asset = std::make_shared<Asset>(path);
				if (asset->Load() == false)
				{
					// Either not a HodEngine asset file (e.g. .DS_Store, Thumbs.db, ...) or a genuinely
					// corrupted/unsupported one; Asset::Load() already logged in the latter case.
					continue;
				}

				for (const AssetContainer::SourceInfo& source : asset->GetSources())
				{
					_sourcePathToAssetMap.emplace(source._path, asset);
				}

				_uidToAssetMap.emplace(asset->GetUid(), asset);

				FileSystemMapping* childFileSystemMapping = DefaultAllocator::GetInstance().New<FileSystemMapping>();
				childFileSystemMapping->_path = path;
				childFileSystemMapping->_parentFolder = fileSystemMapping;
				childFileSystemMapping->_asset = asset;
				childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;

				// fileSystemMapping->_childrenAsset.PushBack(&childFileSystemMapping->_childrenAsset);
				fileSystemMapping->_childrenAsset.push_back(childFileSystemMapping);
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
		if (_asset != nullptr)
		{
			_asset->SetPath(_path); // Otherwise the assets of a moved folder would still be loaded and saved at their old location
		}

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

	/// @brief
	/// @param assetType
	/// @return
	std::string_view AssetDatabase::GetAssetTypeName(uint64_t assetType) const
	{
		for (const CookerEntry* entry : _cookerEntries)
		{
			if (entry->_assetType == assetType)
			{
				return std::string_view(entry->_assetTypeName.CStr(), entry->_assetTypeName.Length());
			}
		}
		return std::string_view();
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
	/// @param assetType
	void AssetDatabase::UnregisterCooker(std::string_view assetType)
	{
		const uint64_t type = Hash::ComputeXxh3_64(assetType);
		for (uint32_t index = 0; index < _cookerEntries.Size(); ++index)
		{
			if (_cookerEntries[index]->_assetType == type)
			{
				DefaultAllocator::GetInstance().Delete(_cookerEntries[index]);
				_cookerEntries.Erase(index);
				return;
			}
		}
	}

	/// @brief
	/// @param extension
	void AssetDatabase::UnregisterImporter(std::string_view extension)
	{
		const String extensionLower = ToLowerCopy(extension);
		for (uint32_t index = 0; index < _importerEntries.Size(); ++index)
		{
			for (const String& supportedExtension : _importerEntries[index]->_extensions)
			{
				if (supportedExtension == extensionLower)
				{
					DefaultAllocator::GetInstance().Delete(_importerEntries[index]);
					_importerEntries.Erase(index);
					return;
				}
			}
		}
	}

	/// @brief
	/// @param assetType
	void AssetDatabase::UnregisterResource(std::string_view assetType)
	{
		const uint64_t type = Hash::ComputeXxh3_64(assetType);
		for (uint32_t index = 0; index < _resourceEntries.Size(); ++index)
		{
			if (_resourceEntries[index]->_assetType == type)
			{
				DefaultAllocator::GetInstance().Delete(_resourceEntries[index]);
				_resourceEntries.Erase(index);
				return;
			}
		}
	}

	/// @brief
	/// @param result
	/// @param from
	/// @param resourceDescriptor
	void AssetDatabase::ListAsset(Vector<FileSystemMapping*>& result, const FileSystemMapping& from, const ReflectionDescriptor& resourceDescriptor, uint64_t expectedSubType)
	{
		uint64_t assetType = 0;
		for (const ResourceEntry* resourceEntry : _resourceEntries)
		{
			if (resourceEntry->_resourceDescriptor == &resourceDescriptor)
			{
				assetType = resourceEntry->_assetType;
				break;
			}
		}
		if (assetType == 0)
		{
			OUTPUT_ERROR("AssetDatabase::ListAsset: No assetType associated to resourceDescriptor");
			return;
		}

		for (FileSystemMapping* assetNode : from._childrenAsset)
		{
			if (assetType == assetNode->_asset->GetType() && (expectedSubType == 0 || assetNode->_asset->GetSubType() == expectedSubType))
			{
				result.push_back(assetNode);
			}
		}

		for (FileSystemMapping* folderNode : from._childrenFolder)
		{
			ListAsset(result, *folderNode, resourceDescriptor, expectedSubType);
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
					// Either not a HodEngine asset file (e.g. .DS_Store, Thumbs.db, ...) or a genuinely
					// corrupted/unsupported one; Asset::Load() already logged in the latter case.
					return;
				}

				for (const AssetContainer::SourceInfo& source : asset->GetSources())
				{
					_sourcePathToAssetMap.emplace(source._path, asset);
				}

				_uidToAssetMap.emplace(asset->GetUid(), asset);

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
				for (const AssetContainer::SourceInfo& source : asset->GetSources())
				{
					auto range = _sourcePathToAssetMap.equal_range(source._path);
					for (auto it = range.first; it != range.second; ++it)
					{
						if (it->second == asset)
						{
							_sourcePathToAssetMap.erase(it);
							break;
						}
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
					Vector<AssetContainer::SourceInfo> oldSources = asset->GetSources();

					if (asset->Load() == false)
					{
						// Kept in the database: the file is still there, only a removal from disk removes the asset
						OUTPUT_ERROR("Unable to load Asset : {}", path);
						return;
					}

					const Vector<AssetContainer::SourceInfo>& newSources = asset->GetSources();

					for (const AssetContainer::SourceInfo& oldSource : oldSources)
					{
						bool stillPresent = false;
						for (const AssetContainer::SourceInfo& newSource : newSources)
						{
							if (newSource._path == oldSource._path)
							{
								stillPresent = true;
								break;
							}
						}
						if (stillPresent == false)
						{
							auto range = _sourcePathToAssetMap.equal_range(oldSource._path);
							for (auto it = range.first; it != range.second; ++it)
							{
								if (it->second == asset)
								{
									_sourcePathToAssetMap.erase(it);
									break;
								}
							}
						}
					}

					for (const AssetContainer::SourceInfo& newSource : newSources)
					{
						bool wasPresent = false;
						for (const AssetContainer::SourceInfo& oldSource : oldSources)
						{
							if (oldSource._path == newSource._path)
							{
								wasPresent = true;
								break;
							}
						}
						if (wasPresent == false)
						{
							_sourcePathToAssetMap.emplace(newSource._path, asset);
						}
					}

					_uidToAssetMap[asset->GetUid()] = asset;
				}
			}
		}
	}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherSourceOnDeleteFile(const Path& /*path*/) {}

	/// @brief
	/// @param path
	void AssetDatabase::FileSystemWatcherSourceOnChangeFile(const Path& path)
	{
		auto range = _sourcePathToAssetMap.equal_range(path);
		for (auto it = range.first; it != range.second; ++it)
		{
			// TODO reimport
		}
	}

}
