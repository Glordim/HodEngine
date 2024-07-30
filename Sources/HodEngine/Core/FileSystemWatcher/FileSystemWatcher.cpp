#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp"
#include "HodEngine/Core/Frame/FrameSequencer.hpp"

namespace hod
{
	/// @brief 
	FileSystemWatcher::FileSystemWatcher()
	: _internalJob(this, &FileSystemWatcher::Update, JobQueue::FramedLowPriority)
	{

	}

	/// @brief 
	FileSystemWatcher::~FileSystemWatcher()
	{
		Cleanup();
	}

	/// @brief 
	/// @param path 
	/// @param onCreateFile 
	/// @param onDeleteFile 
	/// @param onMoveFile 
	/// @return 
	bool FileSystemWatcher::Init(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& onCreateFile, const std::function<void(const std::filesystem::path&)>& onDeleteFile, const std::function<void(const std::filesystem::path&)>& onChangeFile, const std::function<void(const std::filesystem::path& old, const std::filesystem::path&)>& onMoveFile)
	{
		_path = path;
		_onCreateFile = onCreateFile;
		_onDeleteFile = onDeleteFile;
		_onChangeFile = onChangeFile;
		_onMoveFile = onMoveFile;

		if (std::filesystem::is_directory(_path))
		{
			if (std::filesystem::exists(_path) == false)
			{
				return false; // todo output
			}
		}
		else
		{
			if (std::filesystem::exists(_path.parent_path()) == false)
			{
				return false; // todo output
			}
		}

		return InternalInit();
	}

	/// @brief 
	void FileSystemWatcher::RegisterUpdateJob()
	{
		FrameSequencer::GetInstance()->InsertJob(&_internalJob, FrameSequencer::Step::PreRender);
	}

	/// @brief 
	void FileSystemWatcher::UnregisterUpdateJob()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_internalJob, FrameSequencer::Step::PreRender);
	}
}
