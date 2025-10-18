#pragma once
#include "HodEngine/Core/Export.hpp"

#include <filesystem>

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

#define HOD_DECLARE_MODULE(Name, API) \
	extern "C"                        \
	{                                 \
		API int StartupModule();      \
		API int ShutdownModule();     \
	}

#define HOD_STARTUP_MODULE(Name) int StartupModule()
#define HOD_SHUTDOWN_MODULE(Name) int ShutdownModule()

namespace hod
{
	class HOD_CORE_API Module
	{
	public:
		Module() = default;
		~Module();

		void Init(const std::filesystem::path& path, bool copyForSupportReload);

		bool Load();
		bool Unload();
		bool Reload();

		const std::filesystem::path& GetPath() const;

	private:
		bool InternalLoad(const std::filesystem::path& path);
		bool InternalUnload();

		const char* GetModuleExtension();
		const char* GetModulePrefix();

	private:
		std::filesystem::path _path;
		std::filesystem::path _copyPath;
		bool                  _copyForSupportReload;

#if defined(PLATFORM_WINDOWS)
		HINSTANCE _dll = NULL;
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)
		void* _sharedLib = nullptr;
#endif
	};
}
