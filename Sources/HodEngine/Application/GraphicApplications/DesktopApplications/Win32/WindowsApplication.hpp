#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::inline application
{
	/// @brief
	class HOD_APPLICATION_API WindowsApplication : public DesktopApplication
	{
		_SingletonOverride(WindowsApplication)
	};
}
