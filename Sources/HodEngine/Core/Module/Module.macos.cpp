#include "HodEngine/Core/Module/Module.hpp"

 #include <dlfcn.h>

namespace hod
{
	/// @brief 
	/// @return 
	bool Module::InternalLoad(const std::filesystem::path& path)
	{
		_sharedLib = dlopen(path.string().c_str(), RTLD_LAZY);
		if (_sharedLib == nullptr)
		{
			//std::cout << "could not load the dynamic library" << std::endl;
			return false;
		}

		using initFunction = int(*)();

		// resolve function address here
		initFunction initFunc = (initFunction)dlsym(_sharedLib, "Init");
		if (initFunc == nullptr)
		{
			//std::cout << "could not locate the function" << std::endl;
			return false;
		}

		initFunc();

		return true;
	}
	
	/// @brief 
	/// @return 
	bool Module::InternaUnload()
	{
		if (_sharedLib != nullptr)
		{
			using cleanFunction = int(*)();

			cleanFunction cleanFunc = (cleanFunction)dlsym(_sharedLib, "Clean");
			if (cleanFunc == nullptr)
			{
				//std::cout << "could not locate the function" << std::endl;
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
		return ".dylib";
	}
}
