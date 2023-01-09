#pragma once

#include <HodEngine/Core/Src/Singleton.h>

namespace hod::application
{
	/// @brief 
	class Application
	{
		_Singleton(Application)

						Application() = default;

	public:

		virtual bool	Init(int argc, char** argv);
		virtual bool	Run();

		void			Quit();

	protected:

		bool			_shouldQuit = false;
	};
}
