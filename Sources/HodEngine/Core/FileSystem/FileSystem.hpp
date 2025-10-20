#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Singleton.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>

#if defined(PLATFORM_ANDROID)
struct AAssetManager;
struct AAsset;
#elif defined(PLATFORM_WINDOWS)
// #include <Windows.h>
typedef void* HANDLE;
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
			AAsset* _asset = nullptr;
#elif defined(PLATFORM_WINDOWS)
			HANDLE _handle = (HANDLE)-1;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
			int _fd = -1;
#else
			FILE* _file = nullptr;
#endif

			Path _path; // TODO REMOVE

			bool IsOpen() const;
		};

#if defined(PLATFORM_WINDOWS)
		typedef struct _FILETIME
		{
			uint32_t dwLowDateTime;
			uint32_t dwHighDateTime;
		} FILETIME, *PFILETIME, *LPFILETIME;

		using FileTime = FILETIME;
#endif

	public:
		static Path GetUserSettingsPath();
		static Path GetExecutablePath();
		static Path GetTemporaryPath();
		static bool SetWorkingDirectory(const Path& path);

	private:
		static Path _userSettingsPath;
		static Path _temporaryPath;
		static Path _executablePath;

	public:
#if defined(PLATFORM_ANDROID)
		bool Init(AAssetManager* assetManager);
#endif
		bool Init();

		bool IsRegularFile(const char* path);
		bool IsRegularFile(const Path& path);

		bool CopyFile(const char* pathSrc, const char* pathDst, bool overwrite = false);
		bool CopyFile(const Path& pathSrc, const Path& pathDst, bool overwrite = false);

		bool RemoveAll(const char* path);
		bool RemoveAll(const Path& path);

		bool Remove(const char* path);
		bool Remove(const Path& path);

		bool Rename(const char* pathSrc, const char* pathDst);
		bool Rename(const Path& pathSrc, const Path& pathDst);

		bool Exists(const char* path);
		bool Exists(const Path& path);

		bool IsDirectory(const char* path);
		bool IsDirectory(const Path& path);

		bool CreateDirectories(const char* path);
		bool CreateDirectories(const Path& path);

		FileSystem::Handle Open(const char* path);
		FileSystem::Handle Open(const Path& path);

		uint32_t GetSize(FileSystem::Handle handle);
		uint32_t GetOffset(FileSystem::Handle handle);

		void Seek(FileSystem::Handle handle, uint32_t position, SeekMode mode);

		int32_t Read(FileSystem::Handle handle, void* buffer, uint32_t size);

		bool Close(FileSystem::Handle& handle);

		Path GenerateTemporaryFilePath() const;

	private:
#if defined(PLATFORM_ANDROID)
		AAssetManager* _assetManager = nullptr;
#endif
	};
}
