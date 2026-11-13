#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>

#if defined(PLATFORM_WINDOWS)
struct HINSTANCE__;
#endif

namespace hod::inline core
{
	class HOD_CORE_API DynamicLibrary
	{
	public:
		DynamicLibrary() = default;
		~DynamicLibrary();

		/// @brief The file of the library at path (no prefix, no extension: "Bin/Physics" -> "Bin/Physics.dll", "Bin/libPhysics.so"...)
		static Path GetFilePath(const Path& path);

		/// @param path No prefix, no extension (see GetFilePath)
		/// @param copyForSupportReload Loads a copy (<Name>-Copy), the file itself can be rebuilt meanwhile
		void Init(const Path& path, bool copyForSupportReload);

		bool Load();
		bool Unload();
		bool Reload();

		const Path& GetPath() const;

		template<typename FuncType>
		FuncType LoadFunction(const char* name) const
		{
			return reinterpret_cast<FuncType>(GetFunctionInternal(name));
		}

	private:
		bool InternalLoad(const Path& path);
		bool InternalUnload();

		void* GetFunctionInternal(const char* name) const;

		static const char* GetModuleExtension();
		static const char* GetModulePrefix();

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
