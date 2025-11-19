#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Module/Module.hpp"

#include <dlfcn.h>

namespace hod
{
	/// @brief
	/// @return
	bool Module::InternalLoad(const Path& path)
	{
		_sharedLib = dlopen(path.GetString().CStr(), RTLD_LAZY);
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
	bool Module::InternalUnload()
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
	const char* Module::GetModuleExtension()
	{
		return ".so";
	}

	/// @brief
	/// @return
	const char* Module::GetModulePrefix()
	{
		return "lib";
	}
}
