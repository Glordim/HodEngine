#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <android/asset_manager.h>

namespace hod
{
	std::filesystem::path FileSystem::_userSettingsPath;
	std::filesystem::path FileSystem::_executablePath;
	std::filesystem::path FileSystem::_temporaryPath;

	std::filesystem::path FileSystem::GetUserSettingsPath()
	{
		return FileSystem::_userSettingsPath;
	}

	std::filesystem::path FileSystem::GetExecutablePath()
	{
		return FileSystem::_executablePath;
	}

	std::filesystem::path FileSystem::GetTemporaryPath()
	{
		if (FileSystem::_temporaryPath.empty() == true)
		{
			assert(false); // TODO
		}
		return FileSystem::_temporaryPath;
	}

	bool FileSystem::SetWorkingDirectory(const std::filesystem::path& path)
	{
		return false;
	}

	bool FileSystem::Init(AAssetManager* assetManager)
	{
		_assetManager = assetManager;
		return true;
	}

	FileSystem::Handle FileSystem::Open(const char* path)
	{
		FileSystem::Handle handle;
		handle._asset = AAssetManager_open(_assetManager, path, AASSET_MODE_BUFFER);
		return handle;
	}

	uint32_t FileSystem::GetSize(FileSystem::Handle handle)
	{
		return (uint32_t)AAsset_getLength(handle._asset);
	}

	uint32_t FileSystem::GetOffset(FileSystem::Handle handle)
	{
		return (uint32_t)AAsset_seek(handle._asset, 0, static_cast<int>(SeekMode::Current));
	}

	void FileSystem::Seek(FileSystem::Handle handle, uint32_t position, SeekMode mode)
	{
		AAsset_seek(handle._asset, position, static_cast<int>(mode));
	}

	int32_t FileSystem::Read(FileSystem::Handle handle, void* buffer, uint32_t size)
	{
		return AAsset_read(handle._asset, buffer, size);
	}

	bool FileSystem::Close(FileSystem::Handle& handle)
	{
		AAsset_close(handle._asset);
		handle._asset = nullptr;
		return true;
	}

	bool FileSystem::Handle::IsOpen() const
	{
		return _asset != nullptr;
	}
}
