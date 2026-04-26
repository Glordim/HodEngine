#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"

#include <win32/misc.h>

namespace hod::inline core
{
	/// @brief
	/// @return
	bool DynamicLibrary::InternalLoad(const Path& path)
	{
		_dll = (HINSTANCE__*)LoadLibraryW(path.ToNative().c_str());
		if (_dll == NULL)
		{
			OUTPUT_ERROR("Unable to load module {}", path);
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	bool DynamicLibrary::InternalUnload()
	{
		if (_dll)
		{
			FreeLibrary(_dll);
			_dll = NULL;
		}
		return true;
	}

	/// @brief
	/// @return
	void* DynamicLibrary::GetFunctionInternal(const char* name) const
	{
		return (void*)GetProcAddress(_dll, name);
	}

	/// @brief
	/// @return
	const char* DynamicLibrary::GetModuleExtension()
	{
		return ".dll";
	}

	/// @brief
	/// @return
	const char* DynamicLibrary::GetModulePrefix()
	{
		return "";
	}
}
