#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

namespace hod
{
	_SingletonConstructor(FileSystem)
	{

	}

	bool FileSystem::Init()
	{
		return true;
	}

	FileSystem::Handle FileSystem::Open(const std::filesystem::path& path)
	{
		return Open(path.string().c_str());
	}
}
