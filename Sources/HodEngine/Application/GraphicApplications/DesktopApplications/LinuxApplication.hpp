#pragma once
#include <HodEngine/HodEngine.hpp>
#if defined(PLATFORM_LINUX)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_API LinuxApplication : public DesktopApplication
	{
		_SingletonOverride(LinuxApplication)
	};
}

#endif
