#pragma once

#include <HodEngine/Core/Src/Singleton.h>

namespace HOD
{
	/// @brief 
	class Application
	{
		_Singleton(Application)

						Application() = default;

	public:

		virtual bool	Init(int argc, char** argv);
		virtual bool	Run();

	private:

		bool			_shouldQuit = false;
	};
}
