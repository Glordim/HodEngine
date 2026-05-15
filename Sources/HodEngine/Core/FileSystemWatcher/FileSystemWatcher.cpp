#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

namespace hod::inline core
{
	/// @brief
	FileSystemWatcher::FileSystemWatcher()
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
		if (FileSystem::GetInstance()->IsDirectory(_path))
		{
			_watchingPath = _path;
		}
		else
		{
			_isFile = true;
			_watchingPath = _path.ParentPath();
		}

		_onCreateFile = onCreateFile;
		_onDeleteFile = onDeleteFile;
		_onChangeFile = onChangeFile;
		_onMoveFile = onMoveFile;

		if (FileSystem::GetInstance()->Exists(_watchingPath) == false)
		{
			OUTPUT_ERROR("FileSystemWatcher::Init fail, {} doesn't exist", _watchingPath);
			return false;
		}

		return InternalInit();
	}
}
