#pragma once
#include "HodEngine/Core/Export.hpp"

#if defined(PLATFORM_WINDOWS)
using HANDLE = void*;
using DWORD = unsigned long;
struct _OVERLAPPED;
#elif defined(PLATFORM_MACOS)
struct __FSEventStream;
using FSEventStreamRef = struct __FSEventStream*;
struct dispatch_queue_s;
using dispatch_queue_t = struct dispatch_queue_s*;
#endif

#include <functional>
#include <HodEngine/Core/FileSystem/Path.hpp>

#if defined(PLATFORM_MACOS)
	#include <mutex>
	#include <HodEngine/Core/Vector.hpp>
#endif

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

#if defined(PLATFORM_MACOS)
		/// @brief Called back from the FSEventStream C callback trampoline, on the watcher's dispatch queue thread.
		/// Only buffers the events; they get processed on the calling thread by the next Update().
		void OnFSEvents(size_t numEvents, char** eventPaths, const uint32_t* eventFlags, const uint64_t* eventIds);
#endif

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
		struct FSEventEntry
		{
			Path     path;
			uint32_t flags;
		};

		FSEventStreamRef     _stream = nullptr;
		dispatch_queue_t     _dispatchQueue = nullptr;
		std::mutex           _pendingFSEventsMutex;
		Vector<FSEventEntry> _pendingFSEvents;
#endif
	};
}
