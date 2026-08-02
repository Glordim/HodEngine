#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystemWatcher/FileSystemWatcher.hpp"
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>

#include <CoreServices/CoreServices.h>
#include <dispatch/dispatch.h>

namespace hod::inline core
{
	namespace
	{
		/// @brief Trampoline required by FSEventStreamCreate (plain C function pointer, no captures).
		void FSEventsCallback(ConstFSEventStreamRef /*streamRef*/, void* clientCallBackInfo, size_t numEvents, void* eventPaths,
		                      const FSEventStreamEventFlags eventFlags[], const FSEventStreamEventId eventIds[])
		{
			FileSystemWatcher* watcher = static_cast<FileSystemWatcher*>(clientCallBackInfo);
			watcher->OnFSEvents(numEvents, static_cast<char**>(eventPaths), eventFlags, eventIds);
		}
	}

	/// @brief
	/// @return
	bool FileSystemWatcher::InternalInit()
	{
		CFStringRef watchingPathStr = CFStringCreateWithCString(kCFAllocatorDefault, _watchingPath.CStr(), kCFStringEncodingUTF8);
		CFArrayRef  pathsToWatch = CFArrayCreate(kCFAllocatorDefault, (const void**)&watchingPathStr, 1, &kCFTypeArrayCallBacks);

		FSEventStreamContext context = { 0, this, nullptr, nullptr, nullptr };

		_stream = FSEventStreamCreate(kCFAllocatorDefault, &FSEventsCallback, &context, pathsToWatch, kFSEventStreamEventIdSinceNow, 0.1,
		                              kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer);

		CFRelease(pathsToWatch);
		CFRelease(watchingPathStr);

		if (_stream == nullptr)
		{
			OUTPUT_ERROR("FileSystemWatcher::InternalInit, FSEventStreamCreate failed for {}", _watchingPath);
			return false;
		}

		_dispatchQueue = dispatch_queue_create("HodEngine.FileSystemWatcher", DISPATCH_QUEUE_SERIAL);
		FSEventStreamSetDispatchQueue(_stream, _dispatchQueue);

		if (FSEventStreamStart(_stream) == false)
		{
			OUTPUT_ERROR("FileSystemWatcher::InternalInit, FSEventStreamStart failed for {}", _watchingPath);
			FSEventStreamSetDispatchQueue(_stream, nullptr);
			FSEventStreamRelease(_stream);
			_stream = nullptr;
			dispatch_release(_dispatchQueue);
			_dispatchQueue = nullptr;
			return false;
		}

		return true;
	}

	/// @brief
	void FileSystemWatcher::Cleanup()
	{
		if (_stream != nullptr)
		{
			FSEventStreamStop(_stream);
			FSEventStreamSetDispatchQueue(_stream, nullptr);
			FSEventStreamRelease(_stream);
			_stream = nullptr;
		}

		if (_dispatchQueue != nullptr)
		{
			dispatch_release(_dispatchQueue);
			_dispatchQueue = nullptr;
		}

		_pendingFSEvents.Clear();
	}

	/// @brief Buffers the raw events; the FSEvents callback runs on the watcher's dispatch queue thread, not the
	/// caller's, so the user callbacks (which the caller expects to run synchronously from Update()) must not be
	/// invoked from here directly.
	void FileSystemWatcher::OnFSEvents(size_t numEvents, char** eventPaths, const uint32_t* eventFlags, const uint64_t* /*eventIds*/)
	{
		std::lock_guard<std::mutex> lock(_pendingFSEventsMutex);
		_pendingFSEvents.Reserve(_pendingFSEvents.Size() + static_cast<uint32_t>(numEvents));
		for (size_t i = 0; i < numEvents; ++i)
		{
			_pendingFSEvents.PushBack({ Path(eventPaths[i]), eventFlags[i] });
		}
	}

	/// @brief
	void FileSystemWatcher::Update()
	{
		Vector<FSEventEntry> events;
		{
			std::lock_guard<std::mutex> lock(_pendingFSEventsMutex);
			if (_pendingFSEvents.Empty())
			{
				return;
			}
			events.Swap(_pendingFSEvents);
		}

		Path oldFilePathToRename;

		for (const FSEventEntry& fsEvent : events)
		{
			const Path&    filePath = fsEvent.path;
			const uint32_t flags = fsEvent.flags;

			if ((flags & (kFSEventStreamEventFlagMustScanSubDirs | kFSEventStreamEventFlagUserDropped | kFSEventStreamEventFlagKernelDropped)) != 0)
			{
				if (_onOverflow != nullptr)
				{
					_onOverflow();
				}
				else
				{
					OUTPUT_ERROR("FileSystemWatcher::Update, notification buffer overflow on {}, some file changes may have been missed", _watchingPath);
				}
				continue;
			}

			if (_isFile == false || (flags & kFSEventStreamEventFlagItemRenamed) != 0 || _path == filePath)
			{
				if ((flags & kFSEventStreamEventFlagItemRenamed) != 0)
				{
					if (FileSystem::GetInstance()->Exists(filePath) == false)
					{
						// Old name of the rename pair, kept until the matching new name event arrives.
						oldFilePathToRename = filePath;
					}
					else if (oldFilePathToRename.Empty() == false)
					{
						if (_onMoveFile != nullptr)
						{
							_onMoveFile(oldFilePathToRename, filePath);
						}
						oldFilePathToRename = Path();
					}
					else if (_onCreateFile != nullptr)
					{
						// Renamed into the watched scope from outside it: no old name was seen, treat as a creation.
						_onCreateFile(filePath);
					}
				}
				else if ((flags & kFSEventStreamEventFlagItemCreated) != 0)
				{
					if (_onCreateFile != nullptr)
					{
						_onCreateFile(filePath);
					}
				}
				else if ((flags & kFSEventStreamEventFlagItemRemoved) != 0)
				{
					if (_onDeleteFile != nullptr)
					{
						_onDeleteFile(filePath);
					}
				}
				else if ((flags & (kFSEventStreamEventFlagItemModified | kFSEventStreamEventFlagItemInodeMetaMod)) != 0)
				{
					if (_onChangeFile != nullptr)
					{
						_onChangeFile(filePath);
					}
				}
			}
		}

		if (oldFilePathToRename.Empty() == false)
		{
			// Renamed out of the watched scope: no matching new name event arrived, treat as a deletion.
			if (_onDeleteFile != nullptr)
			{
				_onDeleteFile(oldFilePathToRename);
			}
		}
	}
}
