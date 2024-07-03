#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/Core/Singleton.hpp>

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

		void			Quit();

	protected:

		bool			_shouldQuit = false;
	};
}
