#pragma once
#if defined(PLATFORM_MACOS)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class MacOsApplication : public DesktopApplication
	{
		_SingletonOverride(MacOsApplication)
	};
}

#endif
