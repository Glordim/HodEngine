#pragma once

#include <HodEngine/Core/Singleton.h>

namespace hod
{
	class ArgumentParser;
}

namespace hod::application
{
	/// @brief 
	class Application
	{
		_Singleton(Application)

	public:

		virtual bool	Init(const ArgumentParser& argumentParser);
		virtual bool	Run();

		void			Quit();

	protected:

		bool			_shouldQuit = false;
	};
}
