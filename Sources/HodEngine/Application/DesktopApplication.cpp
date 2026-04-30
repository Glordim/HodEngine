#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/DesktopApplication.hpp"

namespace hod::inline application
{
	bool DesktopApplication::Run(int /*argc*/, char** /*argv*/)
	{
		// todo use argv for config
		return RunInternal();
	}
}
