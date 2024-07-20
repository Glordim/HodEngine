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
		std::wstring assetFolderPath;
		if (std::filesystem::is_directory(_path))
		{
			_dirPath = _path;			
		}
		else
		{
			_isFile = true;
			_dirPath = _path.parent_path();
		}
		StringConversion::StringToWString(_dirPath.string(), assetFolderPath);
		_hDir = CreateFileW(assetFolderPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  		_overlapped.hEvent = CreateEventW(NULL, FALSE, 0, NULL);

		::ReadDirectoryChangesW(_hDir, _changeBuf, sizeof(_changeBuf), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &_overlapped, NULL);

		return true;
	}

	/// @brief 
	void FileSystemWatcher::Cleanup()
	{
		::CloseHandle(_hDir);
	}

	/// @brief 
	void FileSystemWatcher::Update()
	{
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

				std::filesystem::path filePath = _dirPath / relativefilePath;
				if (_isFile == false || fni->Action == FILE_ACTION_RENAMED_NEW_NAME || _path == filePath)
				{
					switch (fni->Action)
					{
						case FILE_ACTION_ADDED:
						{
							if (_onCreateFile != nullptr)
							{
								_onCreateFile(filePath);
							}
						}
						break;

						case FILE_ACTION_REMOVED:
						{
							if (_onDeleteFile != nullptr)
							{
								_onDeleteFile(filePath);
							}
						}
						break;

						case FILE_ACTION_MODIFIED:
						{
							if (_onChangeFile != nullptr)
							{
								_onChangeFile(filePath);
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
							if (_onMoveFile != nullptr)
							{
								_onMoveFile(oldFilePathToRename, filePath);
							}							
						}
						break;
					}
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
	}
}
