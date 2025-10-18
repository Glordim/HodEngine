#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fcntl.h>
#include <mach-o/dyld.h>
#include <sys/param.h>

namespace hod
{
	std::filesystem::path FileSystem::_temporaryPath;
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;

	std::filesystem::path FileSystem::GetUserSettingsPath()
	{
		if (FileSystem::_userSettingsPath.empty() == true)
		{
			struct passwd* pw = getpwuid(getuid());
			FileSystem::_userSettingsPath = pw->pw_dir;
		}
		return FileSystem::_userSettingsPath;
	}

	std::filesystem::path FileSystem::GetExecutablePath()
	{
		if (FileSystem::_executablePath.empty() == true)
		{
			char     buffer[MAXPATHLEN];
			uint32_t bufferSize = sizeof(buffer);
			if (_NSGetExecutablePath(buffer, &bufferSize) == 0)
			{
				FileSystem::_executablePath = buffer;
			}
		}
		return FileSystem::_executablePath;
	}

	bool FileSystem::SetWorkingDirectory(const std::filesystem::path& path)
	{
		if (chdir(path.string().c_str()) != 0)
		{
			OUTPUT_ERROR("Unable to set working directory");
			return false;
		}
		return true;
	}

	std::filesystem::path FileSystem::GetTemporaryPath()
	{
		if (FileSystem::_temporaryPath.empty() == true)
		{
			FileSystem::_temporaryPath = "/tmp";
		}
		return FileSystem::_temporaryPath;
	}

	/// @brief
	/// @param path
	/// @return
	FileSystem::Handle FileSystem::Open(const char* path)
	{
		FileSystem::Handle handle;
		handle._fd = open(path, O_RDONLY);
		return handle;
	}

	/// @brief
	/// @param handle
	/// @return
	uint32_t FileSystem::GetSize(FileSystem::Handle handle)
	{
		struct stat fileStat;
		if (fstat(handle._fd, &fileStat) == -1)
		{
			return 0;
		}
		return (uint32_t)fileStat.st_size;
	}

	/// @brief
	/// @param handle
	/// @return
	uint32_t FileSystem::GetOffset(FileSystem::Handle handle)
	{
		return (uint32_t)lseek(handle._fd, 0, SEEK_CUR);
	}

	/// @brief
	/// @param handle
	/// @param position
	/// @param mode
	void FileSystem::Seek(FileSystem::Handle handle, uint32_t position, SeekMode mode)
	{
		lseek(handle._fd, position, static_cast<int>(mode));
	}

	/// @brief
	/// @param handle
	/// @param buffer
	/// @param size
	/// @return
	int32_t FileSystem::Read(FileSystem::Handle handle, void* buffer, uint32_t size)
	{
		return (int32_t)read(handle._fd, buffer, size);
	}

	/// @brief
	/// @param handle
	/// @return
	bool FileSystem::Close(FileSystem::Handle& handle)
	{
		if (close(handle._fd) == 0)
		{
			handle._fd = -1;
			return true;
		}
		return false;
	}

	/// @brief
	/// @return
	bool FileSystem::Handle::IsOpen() const
	{
		return _fd >= 0;
	}
}
