#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

#if defined(PLATFORM_WINDOWS)
using HANDLE = void*;
using DWORD = unsigned long;
struct _OVERLAPPED;
#elif defined(PLATFORM_MACOS)
	#include <fcntl.h>
	#include <sys/event.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

#include <functional>
#include <HodEngine/Core/FileSystem/Path.hpp>

namespace hod
{
	class HOD_CORE_API FileSystemWatcher
	{
	public:
		FileSystemWatcher();
		~FileSystemWatcher();

		bool Init(const Path& path, const std::function<void(const Path&)>& onCreateFile, const std::function<void(const Path&)>& onDeleteFile,
		          const std::function<void(const Path&)>& onChangeFile, const std::function<void(const Path& old, const Path&)>& onMoveFile);
		void Cleanup();

		void RegisterUpdateJob();
		void UnregisterUpdateJob();

		void Update();

	private:
		bool InternalInit();

	private:
		Path                                          _path;
		std::function<void(const Path&)>              _onCreateFile;
		std::function<void(const Path&)>              _onDeleteFile;
		std::function<void(const Path&)>              _onChangeFile;
		std::function<void(const Path&, const Path&)> _onMoveFile;

		MemberFunctionJob<FileSystemWatcher> _internalJob;

#if defined(PLATFORM_WINDOWS)
		HANDLE       _hDir = nullptr;
		_OVERLAPPED* _overlapped = nullptr;
		alignas(DWORD) uint8_t _changeBuf[1024];
		bool _isFile = false;
		Path _dirPath;
#elif defined(PLATFORM_MACOS)
		int           _fd = -1;
		int           _kQueue = -1;
		struct kevent _change;
#endif
	};
}
