#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <Windows.h>

namespace hod
{
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
		handle._file = fopen(path, "r");
		return handle;
	}

	/// @brief 
	/// @param handle 
	/// @return 
	uint32_t FileSystem::GetSize(FileSystem::Handle handle)
	{
		uint32_t offset = ftell(handle._file);
		fseek(handle._file, 0, SEEK_END);
		uint32_t size = ftell(handle._file);
		fseek(handle._file, offset, SEEK_SET);
		return size;
	}

	/// @brief 
	/// @param handle 
	/// @return 
	uint32_t FileSystem::GetOffset(FileSystem::Handle handle)
	{
		return (uint32_t)ftell(handle._file);
	}

	/// @brief 
	/// @param handle 
	/// @param position 
	/// @param mode 
	void FileSystem::Seek(FileSystem::Handle handle, uint32_t position, SeekMode mode)
	{
		fseek(handle._file, position, static_cast<int>(mode));
	}

	/// @brief 
	/// @param handle 
	/// @param buffer 
	/// @param size 
	/// @return 
	int32_t FileSystem::Read(FileSystem::Handle handle, void* buffer, uint32_t size)
	{
		return (int32_t)fread(buffer, 1, size, handle._file);
	}

	/// @brief 
	/// @param handle 
	/// @return 
	bool FileSystem::Close(FileSystem::Handle& handle)
	{
		if (fclose(handle._file) == 0)
		{
			handle._file = nullptr;
			return true;
		}
		return false;
	}

	/// @brief 
	/// @return 
	bool FileSystem::Handle::IsOpen() const
	{
		return _file != nullptr;
	}
}
