#pragma once
#include <HodEngine/HodEngine.hpp>
#if defined(PLATFORM_MACOS)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_API MacOsApplication : public DesktopApplication
	{
		_SingletonOverride(MacOsApplication)
	};
}

#endif
