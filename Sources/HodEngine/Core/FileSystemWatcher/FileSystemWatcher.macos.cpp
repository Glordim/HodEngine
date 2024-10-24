#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp"
#include "HodEngine/Core/StringConversion.hpp"
#include <filesystem>

namespace hod
{
	/// @brief 
	/// @param path 
	/// @return 
	bool FileSystemWatcher::InternalInit()
	{
		/*
		_fd = open(_path.string().c_str(), O_RDONLY);
		if (_fd == -1)
		{
			perror("open");
			return 1;
		}

		_kQueue = kqueue();
		if (_kQueue == -1)
		{
			perror("kqueue");
			close(_fd);
			return 1;
		}

		EV_SET(&_change, _fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_WRITE, 0, NULL);
		*/
		return true;
	}

	/// @brief 
	void FileSystemWatcher::Cleanup()
	{
		close(_fd);
    	close(_kQueue);
	}

	/// @brief 
	void FileSystemWatcher::Update()
	{
		return;
		
		struct kevent event;
        int nev = kevent(_kQueue, &_change, 1, &event, 1, NULL);
        if (nev == -1)
		{
            perror("kevent");
            return;
        }

        if (nev > 0)
		{
			if (event.fflags & NOTE_WRITE)
			{
				if (_onChangeFile != nullptr)
				{
					//_onChangeFile(filePath);
				}
			}

			if (event.fflags & NOTE_DELETE)
			{
				if (_onDeleteFile != nullptr)
				{
					//_onDeleteFile(filePath);
				}
			}

			if (event.fflags & NOTE_RENAME)
			{
				if (_onMoveFile != nullptr)
				{
					//_onMoveFile(oldFilePathToRename, filePath);
				}
			}
		}
	}
}
