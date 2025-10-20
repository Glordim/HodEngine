#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <Windows.h>

#undef CopyFile

namespace hod
{
	static_assert(static_cast<DWORD>(FileSystem::SeekMode::Begin) == FILE_BEGIN);
	static_assert(static_cast<DWORD>(FileSystem::SeekMode::Current) == FILE_CURRENT);
	static_assert(static_cast<DWORD>(FileSystem::SeekMode::End) == FILE_END);

	Path FileSystem::_temporaryPath;
	Path FileSystem::_userSettingsPath;
	Path FileSystem::_executablePath;

	Path FileSystem::GetUserSettingsPath()
	{
		if (FileSystem::_userSettingsPath.Empty() == true)
		{
			char          username[256];
			unsigned long usernameLen = 256;
			GetUserNameA(username, &usernameLen);
			FileSystem::_userSettingsPath = (String("C:\\users\\") + username + "\\AppData\\Local").CStr();
		}
		return FileSystem::_userSettingsPath;
	}

	Path FileSystem::GetExecutablePath()
	{
		if (FileSystem::_executablePath.Empty() == true)
		{
			char executablePath[MAX_PATH];
			if (GetModuleFileNameA(nullptr, executablePath, MAX_PATH) != 0)
			{
				FileSystem::_executablePath = executablePath;
			}
		}
		return FileSystem::_executablePath;
	}

	Path FileSystem::GetTemporaryPath()
	{
		if (FileSystem::_temporaryPath.Empty() == true)
		{
			char tempPath[MAX_PATH];
			if (GetTempPath(MAX_PATH, tempPath) != 0)
			{
				FileSystem::_temporaryPath = tempPath;
			}
		}
		return FileSystem::_temporaryPath;
	}

	bool FileSystem::SetWorkingDirectory(const Path& path)
	{
		if (SetCurrentDirectoryA(path.GetString().CStr()) == FALSE)
		{
			OUTPUT_ERROR("Unable to set working directory");
			return false;
		}
		return true;
	}

	/// @brief
	/// @param path
	/// @return
	FileSystem::Handle FileSystem::Open(const char* path)
	{
		FileSystem::Handle handle;
		handle._handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle._handle == INVALID_HANDLE_VALUE)
		{
			OUTPUT_ERROR("Unable to open file at : {}, {}", path, OS::GetLastWin32ErrorMessage());
		}
		return handle;
	}

	/// @brief
	/// @param handle
	/// @return
	uint32_t FileSystem::GetSize(FileSystem::Handle handle)
	{
		return GetFileSize(handle._handle, NULL);
	}

	/// @brief
	/// @param handle
	/// @return
	uint32_t FileSystem::GetOffset(FileSystem::Handle handle)
	{
		return SetFilePointer(handle._handle, 0, NULL, FILE_CURRENT);
	}

	/// @brief
	/// @param handle
	/// @param position
	/// @param mode
	void FileSystem::Seek(FileSystem::Handle handle, uint32_t position, SeekMode mode)
	{
		SetFilePointer(handle._handle, position, NULL, static_cast<DWORD>(mode));
	}

	/// @brief
	/// @param handle
	/// @param buffer
	/// @param size
	/// @return
	int32_t FileSystem::Read(FileSystem::Handle handle, void* buffer, uint32_t size)
	{
		DWORD readedBytes = 0;
		if (ReadFile(handle._handle, buffer, size, &readedBytes, NULL) == TRUE)
		{
			return (int32_t)readedBytes;
		}
		return -1;
	}

	/// @brief
	/// @param handle
	/// @return
	bool FileSystem::Close(FileSystem::Handle& handle)
	{
		if (CloseHandle(handle._handle) == TRUE)
		{
			handle._handle = INVALID_HANDLE_VALUE;
			return true;
		}
		return false;
	}

	/// @brief
	/// @return
	bool FileSystem::Handle::IsOpen() const
	{
		return _handle != INVALID_HANDLE_VALUE;
	}

	bool FileSystem::Exists(const char* path)
	{
		DWORD attr = GetFileAttributesA(path);
		return attr != INVALID_FILE_ATTRIBUTES;
	}

	bool FileSystem::IsDirectory(const char* path)
	{
		DWORD attr = GetFileAttributesA(path);
		if (attr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}
		return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	bool FileSystem::CreateDirectories(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		if (IsDirectory(path))
		{
			return false;
		}

		String cleaned = path;
		if (cleaned.Back() == '\\' || cleaned.Back() == '/')
		{
			cleaned.PopBack();
		}

		size_t pos = cleaned.FindR("\\/");

		if (pos != String::Npos)
		{
			String parent = cleaned.SubStr(0, pos);
			if (!parent.Empty() && !CreateDirectories(parent))
			{
				return false;
			}
		}

		if (CreateDirectoryA(cleaned.CStr(), nullptr))
		{
			return true;
		}

		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS)
		{
			return true;
		}

		return false;
	}

	bool FileSystem::Remove(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		DWORD attr = GetFileAttributesA(path);
		if (attr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		if (attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (RemoveDirectoryA(path))
			{
				return true;
			}
			return false;
		}
		else
		{
			if (DeleteFileA(path))
			{
				return true;
			}

			return false;
		}
	}

	bool FileSystem::RemoveAll(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		DWORD attr = GetFileAttributesA(path);
		if (attr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		if (attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			String searchPath = path;
			if (searchPath.Back() != '\\' && searchPath.Back() != '/')
			{
				searchPath += '\\';
			}
			searchPath += '*';

			WIN32_FIND_DATAA findData;
			HANDLE           hFind = FindFirstFileA(searchPath.CStr(), &findData);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					const char* name = findData.cFileName;

					if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
					{
						continue;
					}

					std::string childPath = path;
					if (childPath.back() != '\\' && childPath.back() != '/')
					{
						childPath += '\\';
					}
					childPath += name;

					RemoveAll(childPath.c_str());

				} while (FindNextFileA(hFind, &findData));

				FindClose(hFind);
			}

			if (!RemoveDirectoryA(path))
			{
				return false;
			}

			return true;
		}
		else
		{
			if (!DeleteFileA(path))
			{
				return false;
			}

			return true;
		}
	}

	bool FileSystem::CopyFile(const char* pathSrc, const char* pathDst, bool overwrite)
	{
		if (pathSrc[0] == '\0' || pathDst[0] == '\0')
		{
			return false;
		}

		DWORD srcAttr = GetFileAttributesA(pathSrc);
		if (srcAttr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		if (srcAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			return false;
		}

		BOOL ok = CopyFileA(pathSrc, pathDst, overwrite ? FALSE : TRUE);
		return ok != 0;
	}

	bool FileSystem::IsRegularFile(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		DWORD attr = GetFileAttributesA(path);
		if (attr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		if ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0 && (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
		{
			return false;
		}

		return true;
	}

	bool FileSystem::Rename(const char* pathSrc, const char* pathDst)
	{
		if (!pathSrc || !pathDst || pathSrc[0] == '\0' || pathDst[0] == '\0')
		{
			return false;
		}

		DWORD attr = GetFileAttributesA(pathSrc);
		if (attr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		BOOL ok = MoveFileExA(pathSrc, pathDst, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);

		return ok != 0;
	}
}
