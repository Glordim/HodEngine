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
			delete pair.second;
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

			if (entry.is_directory() == true)
			{
				childFileSystemMapping->_asset = nullptr;
				ExploreAndDetectAsset(childFileSystemMapping);
			}
			else
			{
				Asset* asset = new Asset(childFileSystemMapping->_path);
				if (asset->Load() == true)
				{
					childFileSystemMapping->_asset = asset;
					_uidToAssetMap.emplace(asset->GetUid(), asset);
				}
				else
				{
					delete asset;
				}
			}

			fileSystemMapping->_children.push_back(childFileSystemMapping);
		}
	}
}
