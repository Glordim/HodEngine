#include "AssetDatabase.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

#include "HodEngine/Editor/src/Editor.h"
#include "HodEngine/Editor/src/Project.h"
#include "HodEngine/Editor/src/Asset.h"

#include <HodEngine/Core/Src/Frame/FrameSequencer.h>

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
		}

		return uniquePath;
	}

	/// @brief 
	AssetDatabase::AssetDatabase()
		: _onProjectLoadedSlot(std::bind(&AssetDatabase::OnProjectLoaded, this, std::placeholders::_1))
		, _onProjectClosedSlot(std::bind(&AssetDatabase::OnProjectClosed, this, std::placeholders::_1))
		, _filesystemWatcherJob(this, &AssetDatabase::FilesystemWatcherJob, JobQueue::FramedLowPriority)
	{
	}

	/// @brief 
	AssetDatabase::~AssetDatabase()
	{
	}

	/// @brief 
	/// @return 
	bool AssetDatabase::Init()
	{
		Editor::GetInstance()->OnProjectLoaded += _onProjectLoadedSlot;
		Editor::GetInstance()->OnProjectClosed += _onProjectClosedSlot;

		return true;
	}

	/// @brief 
	/// @param project  
	void AssetDatabase::OnProjectLoaded(Project* project) 
	{
		_filesystemWatcherHandle = ::FindFirstChangeNotification(project->GetAssetDirPath().string().c_str(), TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_SECURITY);

		_rootFileSystemMapping._asset = nullptr;
		_rootFileSystemMapping._type = FileSystemMapping::FolderType;
		_rootFileSystemMapping._path = project->GetAssetDirPath();
		ExploreAndDetectAsset(&_rootFileSystemMapping);

		FrameSequencer::GetInstance()->InsertJob(&_filesystemWatcherJob, FrameSequencer::Step::PreRender);
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
	/// @param project 
	void AssetDatabase::OnProjectClosed(Project* project)
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

				fileSystemMapping->_childrenFolder.push_back(childFileSystemMapping);
			}
			else
			{
				std::shared_ptr<Asset> asset = std::make_shared<Asset>(childFileSystemMapping->_path);
				if (asset->Load() == true)
				{
					childFileSystemMapping->_asset = asset;
					childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;
					_uidToAssetMap.emplace(asset->GetUid(), asset);

					fileSystemMapping->_childrenAsset.push_back(childFileSystemMapping);
				}
			}
		}
	}

	/// @brief 
	/// @param path 
	/// @return 
	AssetDatabase::FileSystemMapping* AssetDatabase::FindFileSystemMappingFromPath(const std::filesystem::path path) const
	{
		std::filesystem::path rootPath = path.root_path();
		//path.relative_path();
		/*
		for (FileSystemMapping* fileSystemMapping : _filesystemMapping)
		{
			if (fileSystemMapping->_path. )
		}
		*/
		return nullptr;
	}

	/// @brief 
	/// @param folder 
	/// @return 
	const AssetDatabase::FileSystemMapping& AssetDatabase::GetAssetRootNode() const
	{
		return _rootFileSystemMapping;
	}
}
