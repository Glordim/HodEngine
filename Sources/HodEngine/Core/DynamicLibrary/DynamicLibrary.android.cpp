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
			// std::cout << "could not load the dynamic library" << std::endl;
			return false;
		}

		using initFunction = int (*)();

		// resolve function address here
		initFunction initFunc = (initFunction)dlsym(_sharedLib, "StartupModule");
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
		if (_sharedLib != nullptr)
		{
			using cleanFunction = int (*)();

			cleanFunction cleanFunc = (cleanFunction)dlsym(_sharedLib, "ShutdownModule");
			if (cleanFunc == nullptr)
			{
				// std::cout << "could not locate the function" << std::endl;
				return false;
			}

			cleanFunc();

			dlclose(_sharedLib);
			_sharedLib = nullptr;
		}

		return true;
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
