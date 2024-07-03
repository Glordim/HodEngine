#pragma once
#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HODENGINE_API WindowsApplication : public DesktopApplication
	{
		_SingletonOverride(WindowsApplication)
	};
}

#endif
