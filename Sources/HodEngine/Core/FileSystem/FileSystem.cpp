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

	std::filesystem::path FileSystem::GenerateTemporaryFilePath() const
	{
		return GetTemporaryPath() / ("Hod_Tmp_" + UID::GenerateUID().ToString());
	}

	FileSystem::Handle FileSystem::Open(const std::filesystem::path& path)
	{
		FileSystem::Handle handle = Open(path.string().c_str());
		handle._path = path;
		return handle;
	}
}
