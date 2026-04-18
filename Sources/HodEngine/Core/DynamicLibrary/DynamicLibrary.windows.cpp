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
			// std::cout << "could not load the dynamic library" << std::endl;
			OUTPUT_ERROR("Unable to load module {}", path);
			return false;
		}

		using initFunction = int (*)();

		// resolve function address here
		initFunction initFunc = (initFunction)GetProcAddress(_dll, "StartupModule");
		if (initFunc == nullptr)
		{
			// std::cout << "could not locate the function" << std::endl;
			return false;
		}

		initFunc();

		return true;
	}

	/// @brief
	/// @return
	bool DynamicLibrary::InternalUnload()
	{
		if (_dll)
		{
			using cleanFunction = int (*)();

			cleanFunction cleanFunc = (cleanFunction)GetProcAddress(_dll, "ShutdownModule");
			if (cleanFunc == nullptr)
			{
				// std::cout << "could not locate the function" << std::endl;
				return false;
			}

			cleanFunc();

			FreeLibrary(_dll);
			_dll = NULL;
		}

		return true;
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
