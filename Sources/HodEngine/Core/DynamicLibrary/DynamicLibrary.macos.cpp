#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/FileSystem/Path.hpp"
#include <dlfcn.h>

namespace hod::inline core
{
	/// @brief
	/// @return
	bool DynamicLibrary::InternalLoad(const Path& path)
	{
		// RTLD_FIRST: dlsym searches this image only, not the ones it depends on (the modules' entry points have the same name in each library)
		_sharedLib = dlopen(path.CStr(), RTLD_LAZY | RTLD_FIRST);
		if (_sharedLib == nullptr)
		{
			OUTPUT_ERROR("Unable to load module {}", path);
			OUTPUT_FUNCTION_ERROR(dlopen, dlerror());
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	bool DynamicLibrary::InternalUnload()
	{
		if (_sharedLib != nullptr)
		{
			dlclose(_sharedLib);
			_sharedLib = nullptr;
		}
		return true;
	}

	/// @brief
	/// @return
	void* DynamicLibrary::GetFunctionInternal(const char* name) const
	{
		return dlsym(_sharedLib, name);
	}

	/// @brief
	/// @return
	const char* DynamicLibrary::GetModuleExtension()
	{
		return ".dylib";
	}

	/// @brief
	/// @return
	const char* DynamicLibrary::GetModulePrefix()
	{
		return "lib";
	}
}
