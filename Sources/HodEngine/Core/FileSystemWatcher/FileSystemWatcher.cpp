#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp"
#include "HodEngine/Core/Frame/FrameSequencer.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

namespace hod
{
	/// @brief
	FileSystemWatcher::FileSystemWatcher()
	: _internalJob(this, &FileSystemWatcher::Update, JobQueue::Queue::Framed)
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
	bool FileSystemWatcher::Init(const Path& path, const std::function<void(const Path&)>& onCreateFile, const std::function<void(const Path&)>& onDeleteFile,
	                             const std::function<void(const Path&)>& onChangeFile, const std::function<void(const Path& old, const Path&)>& onMoveFile)
	{
		_path = path;
		_onCreateFile = onCreateFile;
		_onDeleteFile = onDeleteFile;
		_onChangeFile = onChangeFile;
		_onMoveFile = onMoveFile;

		if (FileSystem::GetInstance()->IsDirectory(_path))
		{
			if (FileSystem::GetInstance()->Exists(_path) == false)
			{
				return false; // todo output
			}
		}
		else
		{
			if (FileSystem::GetInstance()->Exists(_path.ParentPath()) == false)
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
