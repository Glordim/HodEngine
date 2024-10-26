#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/OS.hpp"

#include <Windows.h>

namespace hod
{
	static_assert(static_cast<DWORD>(FileSystem::SeekMode::Begin) == FILE_BEGIN);
	static_assert(static_cast<DWORD>(FileSystem::SeekMode::Current) == FILE_CURRENT);
	static_assert(static_cast<DWORD>(FileSystem::SeekMode::End) == FILE_END);

	std::filesystem::path FileSystem::_temporaryPath;
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;

	std::filesystem::path FileSystem::GetUserSettingsPath()
	{
		if (FileSystem::_userSettingsPath.empty() == true)
		{
			char username[256];
			unsigned long usernameLen = 256;
			GetUserNameA(username, &usernameLen);
			FileSystem::_userSettingsPath = std::string("C:\\users\\") + username + "\\AppData\\Local";
		}
		return FileSystem::_userSettingsPath;
	}

	std::filesystem::path FileSystem::GetExecutablePath()
	{
		if (FileSystem::_executablePath.empty() == true)
		{
			char executablePath[MAX_PATH];
			if (GetModuleFileNameA(nullptr, executablePath, MAX_PATH) != 0)
			{
				FileSystem::_executablePath = executablePath;
			}
		}
		return FileSystem::_executablePath;
	}

	std::filesystem::path FileSystem::GetTemporaryPath()
	{
		if (FileSystem::_temporaryPath.empty() == true)
		{
			char tempPath[MAX_PATH];
			if (GetTempPath(MAX_PATH, tempPath) != 0)
			{
				FileSystem::_temporaryPath = tempPath;
			}
		}
		return FileSystem::_temporaryPath;
	}

	bool FileSystem::SetWorkingDirectory(const std::filesystem::path& path)
	{
		if (SetCurrentDirectoryA(path.string().c_str()) == FALSE)
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
		handle._handle = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
}
