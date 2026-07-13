#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp"
#include "HodEngine/Core/StringConversion.hpp"
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/OS.hpp>
#include <HodEngine/Core/ScopedGuard.hpp>

/*
#include <win32/file.h>
#include <win32/threads.h>
#include <win32/io.h>
#include <win32/window.h>
#include <win32/windows_base.h>
extern "C"
{
	BOOL WINAPI GetOverlappedResult(_In_ HANDLE hFile, _In_ LPOVERLAPPED lpOverlapped, _Out_ LPDWORD lpNumberOfBytesTransferred, _In_ BOOL bWait);
}
*/
#include <Windows.h>

namespace hod::inline core
{
	bool FileSystemWatcher::InternalInit()
	{
		ScopedGuard cleanup = [&]() {
			Cleanup();
		};

		_hDir = CreateFileW(_watchingPath.ToNative().c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
		                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
		if (_hDir == INVALID_HANDLE_VALUE)
		{
			OUTPUT_FUNCTION_ERROR(CreateFileW, OS::GetLastWin32ErrorMessage());
			return false;
		}

		_overlapped = DefaultAllocator::GetInstance().New<OVERLAPPED>();
		_overlapped->hEvent = CreateEventW(NULL, FALSE, 0, NULL);
		if (_overlapped->hEvent == NULL)
		{
			OUTPUT_FUNCTION_ERROR(CreateEventW, OS::GetLastWin32ErrorMessage());
			return false;
		}

		if (ReadDirectoryChangesW(_hDir, _changeBuf, sizeof(_changeBuf), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL,
		                        _overlapped, NULL) == 0)
		{
			OUTPUT_FUNCTION_ERROR(ReadDirectoryChangesW, OS::GetLastWin32ErrorMessage());
			return false;
		}

		cleanup.Disable();
		return true;
	}

	void FileSystemWatcher::Cleanup()
	{
		if (_hDir != INVALID_HANDLE_VALUE)
		{
			if (_overlapped && _overlapped->hEvent != NULL)
			{
				CancelIoEx(_hDir, _overlapped);
				DWORD bytesTransferred;
				GetOverlappedResult(_hDir, _overlapped, &bytesTransferred, TRUE);
			}
			CloseHandle(_hDir);
			_hDir = INVALID_HANDLE_VALUE;
		}
		if (_overlapped)
		{
			if (_overlapped->hEvent)
			{
				CloseHandle(_overlapped->hEvent);
			}
			DefaultAllocator::GetInstance().Delete(_overlapped);
			_overlapped = nullptr;
		}
	}

	/// @brief
	void FileSystemWatcher::Update()
	{
		DWORD result = WaitForSingleObject(_overlapped->hEvent, 0);

		if (result == WAIT_OBJECT_0)
		{
			DWORD bytesTransferred = 0;
			if (GetOverlappedResult(_hDir, _overlapped, &bytesTransferred, FALSE) && bytesTransferred > 0)
			{
				Path oldFilePathToRename;

				FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)_changeBuf;
				while (true)
				{
					std::wstring result(fni->FileName, fni->FileNameLength / sizeof(wchar_t));
					String       relativefilePath;
					StringConversion::WStringToString(result, relativefilePath);

					Path filePath = _watchingPath / relativefilePath.CStr();
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
			}
			else
			{
				// The notification buffer overflowed: some individual notifications since the last Update() were
				// discarded by the OS and cannot be recovered here. Let the caller reconcile on its own terms.
				if (_onOverflow != nullptr)
				{
					_onOverflow();
				}
				else
				{
					OUTPUT_ERROR("FileSystemWatcher::Update, notification buffer overflow on {}, some file changes may have been missed", _watchingPath);
				}
			}

			::ReadDirectoryChangesW(_hDir, _changeBuf, sizeof(_changeBuf), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL,
			                        _overlapped, NULL);
		}

		// if (::WaitForSingleObject(_filesystemWatcherHandle, 0) == WAIT_OBJECT_0)

		//::FindNextChangeNotification(_filesystemWatcherHandle);
		// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstchangenotificationa
		// https://docs.microsoft.com/en-us/windows/win32/fileio/obtaining-directory-change-notifications
		// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
	}
}
