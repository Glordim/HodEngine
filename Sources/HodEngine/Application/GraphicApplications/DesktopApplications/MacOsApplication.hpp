#pragma once
#include "HodEngine/Application/Export.hpp"
#if defined(PLATFORM_MACOS)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_APPLICATION_API MacOsApplication : public DesktopApplication
	{
		_SingletonOverride(MacOsApplication)
	};
}

#endif
