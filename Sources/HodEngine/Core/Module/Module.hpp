#pragma once
#include "HodEngine/Core/Export.hpp"

#include <filesystem>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace hod
{
	class HOD_CORE_API Module
	{
	public:

		Module(const std::filesystem::path& path, bool copyForSupportReload);
		~Module();

		bool	Load();
		bool	Unload();
		bool	Reload();

		const std::filesystem::path& GetPath() const;

	private:

		bool	InternalLoad(const std::filesystem::path& path);
		bool	InternalUnload();

		const char* GetModuleExtension();
		const char* GetModulePrefix();

	private:

		std::filesystem::path	_path;
		std::filesystem::path	_copyPath;
		bool					_copyForSupportReload;

#if defined(PLATFORM_WINDOWS)
		HINSTANCE		_dll = NULL;
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
		void*			_sharedLib = nullptr;
#endif
	};
}
