#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <fcntl.h>
#include <linux/limits.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace hod
{
	static_assert(static_cast<int>(FileSystem::SeekMode::Begin) == SEEK_SET);
	static_assert(static_cast<int>(FileSystem::SeekMode::Current) == SEEK_CUR);
	static_assert(static_cast<int>(FileSystem::SeekMode::End) == SEEK_END);

	Path FileSystem::_temporaryPath;
	Path FileSystem::_userSettingsPath;
	Path FileSystem::_executablePath;

	Path FileSystem::GetUserSettingsPath()
	{
		if (FileSystem::_userSettingsPath.Empty() == true)
		{
			struct passwd* pw = getpwuid(getuid());
			FileSystem::_userSettingsPath = pw->pw_dir;
		}
		return FileSystem::_userSettingsPath;
	}

	Path FileSystem::GetExecutablePath()
	{
		if (FileSystem::_executablePath.Empty() == true)
		{
			char executablePath[PATH_MAX] = {'\0'}; // init with 0 becausse readlink does not null terminate!
			if (readlink("/proc/self/exe", executablePath, PATH_MAX) == -1)
			{
				perror("readlink");
			}
			else
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
			FileSystem::_temporaryPath = "/tmp";
		}
		return FileSystem::_temporaryPath;
	}

	bool FileSystem::SetWorkingDirectory(const Path& path)
	{
		if (chdir(path.GetString().CStr()) != 0)
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
		return fileStat.st_size;
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
	/// @param Size
	/// @return
	int32_t FileSystem::Read(FileSystem::Handle handle, void* buffer, uint32_t Size)
	{
		return (int32_t)read(handle._fd, buffer, Size);
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
