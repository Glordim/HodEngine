#pragma once
#include "HodEngine/Core/Export.hpp"

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

namespace hod::inline core
{
	class HOD_CORE_API FileSystemWatcher
	{
	public:
		FileSystemWatcher();
		~FileSystemWatcher();

		/// @brief onOverflow is called when the OS notification buffer overflowed: individual create/delete/change/move
		/// notifications have been irrecoverably lost since the last Update(). The watcher does not attempt to recover
		/// on its own (it has no notion of what the caller considers the "true" state) — it is up to the caller to
		/// reconcile, typically by rescanning the watched path with whatever bookkeeping it already maintains.
		bool Init(const Path& path, const std::function<void(const Path&)>& onCreateFile, const std::function<void(const Path&)>& onDeleteFile,
		          const std::function<void(const Path&)>& onChangeFile, const std::function<void(const Path& old, const Path&)>& onMoveFile,
		          const std::function<void()>& onOverflow = nullptr);
		void Cleanup();

		void Update();

	private:
		bool InternalInit();

	private:
		Path                                          _path;
		bool                                          _isFile = false;
		Path                                          _watchingPath;
		std::function<void(const Path&)>              _onCreateFile;
		std::function<void(const Path&)>              _onDeleteFile;
		std::function<void(const Path&)>              _onChangeFile;
		std::function<void(const Path&, const Path&)> _onMoveFile;
		std::function<void()>                         _onOverflow;

		#if defined(PLATFORM_WINDOWS)
		HANDLE       _hDir = nullptr;
		_OVERLAPPED* _overlapped = nullptr;
		alignas(DWORD) uint8_t _changeBuf[64 * 1024];
#elif defined(PLATFORM_MACOS)
		int           _fd = -1;
		int           _kQueue = -1;
		struct kevent _change;
#endif
	};
}
