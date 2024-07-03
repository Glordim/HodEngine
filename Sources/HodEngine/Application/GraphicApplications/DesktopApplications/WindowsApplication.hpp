#pragma once
#include <HodEngine/HodEngine.hpp>
#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_API WindowsApplication : public DesktopApplication
	{
		_SingletonOverride(WindowsApplication)
	};
}

#endif
