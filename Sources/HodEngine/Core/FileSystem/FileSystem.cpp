#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/UID.hpp"

namespace hod::inline core
{
	_SingletonConstructor(FileSystem) {}

#if !defined(PLATFORM_ANDROID)
	bool FileSystem::Init()
	{
		return true;
	}
#endif

	Path FileSystem::GenerateTemporaryFilePath() const
	{
		return GetTemporaryPath() / ("Hod_Tmp_" + UID::GenerateUID().ToString()).CStr(); // Todo remove CStr()
	}

	FileSystem::Handle FileSystem::Open(const Path& path, OpenMode mode)
	{
		FileSystem::Handle handle = Open(path.GetString().CStr(), mode);
		handle._path = path;
		return handle;
	}

	bool FileSystem::Exists(const Path& path)
	{
		return Exists(path.GetString().CStr());
	}

	bool FileSystem::IsDirectory(const Path& path)
	{
		return IsDirectory(path.GetString().CStr());
	}

	bool FileSystem::CreateDirectories(const Path& path)
	{
		return CreateDirectories(path.GetString().CStr());
	}

	bool FileSystem::Remove(const Path& path)
	{
		return Remove(path.GetString().CStr());
	}

	bool FileSystem::RemoveAll(const Path& path)
	{
		return RemoveAll(path.GetString().CStr());
	}

	bool FileSystem::CopyFile(const Path& pathSrc, const Path& pathDst, bool overwrite)
	{
		return CopyFile(pathSrc.GetString().CStr(), pathDst.GetString().CStr(), overwrite);
	}

	bool FileSystem::IsRegularFile(const Path& path)
	{
		return IsRegularFile(path.GetString().CStr());
	}

	bool FileSystem::Rename(const Path& pathSrc, const Path& pathDst)
	{
		return Rename(pathSrc.GetString().CStr(), pathDst.GetString().CStr());
	}

	bool FileSystem::ReadAllText(const Path& path, String& content)
	{
		Handle handle = Open(path, OpenMode::Read);
		if (handle.IsOpen() == false)
		{
			return false;
		}

		uint32_t size = GetSize(handle);
		content.Resize(size);
		int32_t readBytes = Read(handle, content.Data(), size);
		Close(handle);

		return readBytes == (int32_t)size;
	}

	bool FileSystem::WriteAllText(const Path& path, const String& content)
	{
		Handle handle = Open(path, OpenMode::Write);
		if (handle.IsOpen() == false)
		{
			return false;
		}

		int32_t writtenBytes = Write(handle, content.CStr(), content.Length());
		Close(handle);

		return writtenBytes == (int32_t)content.Length();
	}
}
