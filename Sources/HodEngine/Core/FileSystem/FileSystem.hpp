#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Singleton.hpp"

#include <filesystem>

#if defined(PLATFORM_ANDROID)
	struct AAssetManager;
	struct AAsset;
#elif defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#else
	#include <cstdio>
#endif

namespace hod
{
	class HOD_CORE_API FileSystem
	{
		_Singleton(FileSystem)

	public:

		enum class SeekMode
		{
			Begin = 0,
			Current = 1,
			End = 2
		};	

		struct HOD_CORE_API Handle
		{
		#if defined(PLATFORM_ANDROID)
			AAsset*	_asset = nullptr;
		#elif defined(PLATFORM_WINDOWS)
			HANDLE	_handle = INVALID_HANDLE_VALUE;
		#elif defined(PLATFORM_LINUX)
			int		_fd = -1;
		#else
			FILE*	_file = nullptr;
		#endif

			bool IsOpen() const;
		};

	public:

		static std::filesystem::path	GetUserSettingsPath();
		static std::filesystem::path	GetExecutablePath();
		static bool						SetWorkingDirectory(const std::filesystem::path& path);

	private:

		static std::filesystem::path	_userSettingsPath;
		static std::filesystem::path	_executablePath;

	public:

#if defined(PLATFORM_ANDROID)
		bool			Init(AAssetManager* assetManager);
#endif
		bool			Init();

		FileSystem::Handle	Open(const char* path);
		FileSystem::Handle	Open(const std::filesystem::path& path);

		uint32_t			GetSize(FileSystem::Handle handle);
		uint32_t			GetOffset(FileSystem::Handle handle);

		void				Seek(FileSystem::Handle handle, uint32_t position, SeekMode mode);

		int32_t				Read(FileSystem::Handle handle, void* buffer, uint32_t size);

		bool				Close(FileSystem::Handle& handle);

	private:

#if defined(PLATFORM_ANDROID)
		AAssetManager*	_assetManager = nullptr;
#endif
	};
}
