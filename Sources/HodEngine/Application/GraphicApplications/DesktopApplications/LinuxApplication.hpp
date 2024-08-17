#pragma once
#include "HodEngine/Application/Export.hpp"
#if defined(PLATFORM_LINUX)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_APPLICATION_API LinuxApplication : public DesktopApplication
	{
		_SingletonOverride(LinuxApplication)
	};
}

#endif
