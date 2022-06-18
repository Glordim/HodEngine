#pragma once

#include "HodEngine/Application/src/GraphicApplications/DesktopApplications/DesktopApplication.h"

namespace hod::application
{
	/// @brief 
	class WindowsApplication : public DesktopApplication
	{
		_Singleton(WindowsApplication)

						WindowsApplication() = default;

	public:
	};
}
