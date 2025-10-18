#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

#if defined(PLATFORM_WINDOWS)
	#include <WinBase.h>
	#include <Windows.h>
#elif defined(PLATFORM_MACOS)
	#include <fcntl.h>
	#include <sys/event.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

#include <filesystem>
#include <functional>

namespace hod
{
	class HOD_CORE_API FileSystemWatcher
	{
	public:
		FileSystemWatcher();
		~FileSystemWatcher();

		bool Init(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& onCreateFile,
		          const std::function<void(const std::filesystem::path&)>& onDeleteFile, const std::function<void(const std::filesystem::path&)>& onChangeFile,
		          const std::function<void(const std::filesystem::path& old, const std::filesystem::path&)>& onMoveFile);
		void Cleanup();

		void RegisterUpdateJob();
		void UnregisterUpdateJob();

		void Update();

	private:
		bool InternalInit();

	private:
		std::filesystem::path                                                           _path;
		std::function<void(const std::filesystem::path&)>                               _onCreateFile;
		std::function<void(const std::filesystem::path&)>                               _onDeleteFile;
		std::function<void(const std::filesystem::path&)>                               _onChangeFile;
		std::function<void(const std::filesystem::path&, const std::filesystem::path&)> _onMoveFile;

		MemberFunctionJob<FileSystemWatcher> _internalJob;

#if defined(PLATFORM_WINDOWS)
		HANDLE     _hDir = INVALID_HANDLE_VALUE;
		OVERLAPPED _overlapped;
		alignas(DWORD) uint8_t _changeBuf[1024];
		bool                  _isFile = false;
		std::filesystem::path _dirPath;
#elif defined(PLATFORM_MACOS)
		int           _fd = -1;
		int           _kQueue = -1;
		struct kevent _change;
#endif
	};
}
