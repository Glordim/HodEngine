#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/UID.hpp"

namespace hod
{
	_SingletonConstructor(FileSystem) {}

	bool FileSystem::Init()
	{
		return true;
	}

	Path FileSystem::GenerateTemporaryFilePath() const
	{
		return GetTemporaryPath() / ("Hod_Tmp_" + UID::GenerateUID().ToString()).CStr(); // Todo remove CStr()
	}

	FileSystem::Handle FileSystem::Open(const Path& path)
	{
		FileSystem::Handle handle = Open(path.GetString().CStr());
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
}
