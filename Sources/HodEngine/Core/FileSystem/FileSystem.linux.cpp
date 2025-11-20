#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <fcntl.h>
#include <linux/limits.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
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

	bool FileSystem::Exists(const char* path)
	{
		return access(path, F_OK) == 0;
	}

	bool FileSystem::IsDirectory(const char* path)
	{
		struct stat info;
		if (stat(path, &info) != 0)
			return false;

		return S_ISDIR(info.st_mode);
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
		if (!cleaned.Empty() && cleaned.Back() == '/')
			cleaned.PopBack();

		size_t pos = cleaned.FindR('/');
		if (pos != std::string::npos)
		{
			String parent = cleaned.SubStr(0, pos);
			if (!parent.Empty() && !CreateDirectories(parent.CStr()))
				return false;
		}

		if (mkdir(cleaned.CStr(), 0755) == 0)
			return true;

		if (errno == EEXIST)
			return true;

		return false;
	}

	bool FileSystem::Remove(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		struct stat info;
		if (stat(path, &info) != 0)
			return false;

		if (S_ISDIR(info.st_mode))
		{
			if (rmdir(path) == 0)
				return true;
			return false;
		}
		else
		{
			if (unlink(path) == 0)
				return true;
			return false;
		}
	}

	bool FileSystem::RemoveAll(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		struct stat info;
		if (stat(path, &info) != 0)
			return false;

		if (S_ISDIR(info.st_mode))
		{
			DIR* dir = opendir(path);
			if (!dir)
				return false;

			struct dirent* entry;
			while ((entry = readdir(dir)) != nullptr)
			{
				const char* name = entry->d_name;

				if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
				continue;

				std::string childPath = std::string(path) + "/" + name;
				RemoveAll(childPath.c_str());
			}
			closedir(dir);

			if (rmdir(path) != 0)
				return false;

			return true;
		}
		else
		{
			if (unlink(path) != 0)
				return false;
			return true;
		}
	}

	bool FileSystem::CopyFile(const char* pathSrc, const char* pathDst, bool overwrite)
	{
		if (pathSrc[0] == '\0' || pathDst[0] == '\0')
		{
			return false;
		}

		int src = open(pathSrc, O_RDONLY);
		if (src < 0)
			return false;

		struct stat st;
		if (fstat(src, &st) != 0)
		{
			close(src);
			return false;
		}
		if ((st.st_mode & S_IFMT) == S_IFDIR)
		{
			close(src);
			return false;
		}

		int flags = O_WRONLY | O_CREAT;
		if (!overwrite)
			flags |= O_EXCL;

		int dst = open(pathDst, flags, 0644);
		if (dst < 0)
		{
			close(src);
			return false;
		}

		char buffer[4096];
		ssize_t bytes;
		bool ok = true;
		while ((bytes = read(src, buffer, sizeof(buffer))) > 0)
		{
			ssize_t written = write(dst, buffer, bytes);
			if (written != bytes)
			{
				ok = false;
				break;
			}
		}

		if (bytes < 0)
			ok = false;

		close(src);
		close(dst);
		return ok;
	}

	bool FileSystem::IsRegularFile(const char* path)
	{
		if (path[0] == '\0')
		{
			return false;
		}

		struct stat info;
		if (stat(path, &info) != 0)
			return false;

		return S_ISREG(info.st_mode);
	}

	bool FileSystem::Rename(const char* pathSrc, const char* pathDst)
	{
		if (!pathSrc || !pathDst || pathSrc[0] == '\0' || pathDst[0] == '\0')
		{
			return false;
		}

		struct stat info;
		if (stat(pathSrc, &info) != 0)
			return false;

		if (stat(pathDst, &info) == 0)
		{
			if (S_ISDIR(info.st_mode))
			{
				if (rmdir(pathDst) != 0)
					return false;
			}
			else
			{
				if (unlink(pathDst) != 0)
					return false;
			}
		}

		if (rename(pathSrc, pathDst) != 0)
			return false;

		return true;
	}
}
