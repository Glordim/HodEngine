#include "AssetDatabase.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

#include "HodEngine/Editor/src/Editor.h"
#include "HodEngine/Editor/src/Project.h"

#include <HodEngine/Core/Src/Frame/FrameSequencer.h>

#include <Windows.h>

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

		ExploreAndDetectAsset(project->GetAssetDirPath(), nullptr);

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
			delete pair.second;
		}

		_uidToAssetMap.clear();
	}

	/// @brief 
	/// @param dir 
	void AssetDatabase::ExploreAndDetectAsset(const std::filesystem::path dir, FileSystemMapping* parentFileSystemMapping)
	{
		std::filesystem::directory_iterator entries(dir);

		for (const std::filesystem::directory_entry& entry : entries)
		{
			FileSystemMapping* fileSystemMapping = new FileSystemMapping();
			fileSystemMapping->_path = entry.path();
			fileSystemMapping->_lastWriteTime = entry.last_write_time();
			if (entry.is_directory() == true)
			{
				fileSystemMapping->_type = FileSystemMapping::Type::FolderType;
				fileSystemMapping->_uid = UID::INVALID_UID;

				ExploreAndDetectAsset(entry.path(), fileSystemMapping);
			}
			else
			{
				fileSystemMapping->_type = FileSystemMapping::Type::AssetType;
				fileSystemMapping->_uid = UID::INVALID_UID;
			}

			if (parentFileSystemMapping != nullptr)
			{
				parentFileSystemMapping->_children.push_back(fileSystemMapping);
			}
			else
			{
				_filesystemMapping.push_back(fileSystemMapping);
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
	std::vector<std::filesystem::path> AssetDatabase::GetSubFolder(const std::filesystem::path folder) const
	{
		return std::vector<std::filesystem::path>();
		//_filesystemMapping
	}
}
