#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief
	class HOD_APPLICATION_API LinuxApplication : public DesktopApplication
	{
		_SingletonOverride(LinuxApplication)
	};
}
