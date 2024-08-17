#pragma once
#include "HodEngine/Application/Export.hpp"
#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_APPLICATION_API WindowsApplication : public DesktopApplication
	{
		_SingletonOverride(WindowsApplication)
	};
}

#endif
