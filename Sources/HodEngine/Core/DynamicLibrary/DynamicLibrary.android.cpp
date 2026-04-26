#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"

#include <dlfcn.h>

namespace hod::inline core
{
	/// @brief
	/// @return
	bool DynamicLibrary::InternalLoad(const Path& path)
	{
		_sharedLib = dlopen(path.string().c_str(), RTLD_LAZY);
		if (_sharedLib == nullptr)
		{
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
		return ".so";
	}

	/// @brief
	/// @return
	const char* DynamicLibrary::GetModulePrefix()
	{
		return "lib";
	}
}
