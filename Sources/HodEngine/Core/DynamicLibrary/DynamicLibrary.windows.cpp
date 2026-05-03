#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"
#include "HodEngine/Core/OS.hpp"

#include <win32/misc.h>

namespace hod::inline core
{
	/// @brief
	/// @return
	bool DynamicLibrary::InternalLoad(const Path& path)
	{
		_dll = (HINSTANCE__*)LoadLibraryExW(path.ToNative().c_str(), NULL, 0x00000008); //LOAD_WITH_ALTERED_SEARCH_PATH
		if (_dll == NULL)
		{
			OUTPUT_ERROR("Unable to load module {}", path);
			OUTPUT_FUNCTION_ERROR(LoadLibraryW, OS::GetLastWin32ErrorMessage());
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
