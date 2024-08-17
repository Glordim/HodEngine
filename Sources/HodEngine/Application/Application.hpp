#pragma once
#include "HodEngine/Application/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

namespace hod
{
	class ArgumentParser;
}

namespace hod::application
{
	/// @brief 
	class HOD_APPLICATION_API Application
	{
		_Singleton(Application)

	public:

		virtual			~Application() = default;

		virtual bool	Init(const ArgumentParser& argumentParser);
		virtual bool	Run();

		bool			CheckIfGameSharedLibraryExist() const;

		void			Quit();

	protected:

		bool			_shouldQuit = false;
	};
}
