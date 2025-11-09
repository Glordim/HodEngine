#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>

#if defined(PLATFORM_WINDOWS)
struct HINSTANCE__;
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

		void Init(const Path& path, bool copyForSupportReload);

		bool Load();
		bool Unload();
		bool Reload();

		const Path& GetPath() const;

	private:
		bool InternalLoad(const Path& path);
		bool InternalUnload();

		const char* GetModuleExtension();
		const char* GetModulePrefix();

	private:
		Path _path;
		Path _copyPath;
		bool _copyForSupportReload;

#if defined(PLATFORM_WINDOWS)
		HINSTANCE__* _dll = NULL;
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)
		void* _sharedLib = nullptr;
#endif
	};
}
