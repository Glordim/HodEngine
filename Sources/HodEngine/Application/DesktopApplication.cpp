#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/DesktopApplication.hpp"

namespace hod::inline application
{
	bool DesktopApplication::Run(int argc, char** argv)
	{
		_argc = argc;
		_argv = argv;
		return RunInternal();
	}
}
