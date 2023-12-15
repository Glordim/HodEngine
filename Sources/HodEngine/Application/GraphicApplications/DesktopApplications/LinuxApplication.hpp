#pragma once
#if defined(PLATFORM_LINUX)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class LinuxApplication : public DesktopApplication
	{
		_SingletonOverride(LinuxApplication)
	};
}

#endif
