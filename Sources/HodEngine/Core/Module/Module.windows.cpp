#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Module/Module.hpp"

namespace hod
{
	/// @brief
	/// @return
	bool Module::InternalLoad(const Path& path)
	{
		_dll = LoadLibrary(path.GetString().CStr());
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
	bool Module::InternalUnload()
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
	const char* Module::GetModuleExtension()
	{
		return ".dll";
	}

	/// @brief
	/// @return
	const char* Module::GetModulePrefix()
	{
		return "";
	}
}
