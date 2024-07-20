#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#include <WinBase.h>
#endif

#include <functional>
#include <filesystem>

namespace hod
{
	class HOD_API FileSystemWatcher
	{
	public:

		FileSystemWatcher();
		~FileSystemWatcher();

		bool Init(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& onCreateFile, const std::function<void(const std::filesystem::path&)>& onDeleteFile, const std::function<void(const std::filesystem::path&)>& onChangeFile, const std::function<void(const std::filesystem::path& old, const std::filesystem::path&)>& onMoveFile);
		void Cleanup();

		void RegisterUpdateJob();
		void UnregisterUpdateJob();

		void Update();

	private:

		bool InternalInit();

	private:

		std::filesystem::path _path;
		std::function<void(const std::filesystem::path&)> _onCreateFile;
		std::function<void(const std::filesystem::path&)> _onDeleteFile;
		std::function<void(const std::filesystem::path&)> _onChangeFile;
		std::function<void(const std::filesystem::path&, const std::filesystem::path&)> _onMoveFile;

		MemberFunctionJob<FileSystemWatcher>	_internalJob;

#if defined(PLATFORM_WINDOWS)
		HANDLE 					_hDir = NULL;
		OVERLAPPED				_overlapped;
		alignas(DWORD) uint8_t	_changeBuf[1024];
#endif
	};
}
