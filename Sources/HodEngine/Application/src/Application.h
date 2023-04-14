#pragma once

#include <HodEngine/Core/Src/Singleton.h>

namespace hod::core
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

		virtual bool	Init(const core::ArgumentParser& argumentParser);
		virtual bool	Run();

		void			Quit();

	protected:

		bool			_shouldQuit = false;
	};
}
