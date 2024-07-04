#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/Core/Singleton.hpp>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace hod
{
	class ArgumentParser;
}

namespace hod::application
{
	/// @brief 
	class HOD_API Application
	{
		_Singleton(Application)

	public:

		virtual			~Application() = default;

		virtual bool	Init(const ArgumentParser& argumentParser);
		virtual bool	Run();

		virtual bool	LoadDll();
		virtual bool	UnloadDll();
		bool			ReloadDll();

		void			Quit();

	protected:

		bool			_shouldQuit = false;

#if defined(PLATFORM_WINDOWS)
		HINSTANCE		_dll = NULL;
#endif
	};
}
