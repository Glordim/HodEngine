#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::inline application
{
	/// @brief
	class HOD_APPLICATION_API MacOsApplication : public DesktopApplication
	{
		_SingletonOverride(MacOsApplication)
	};
}
