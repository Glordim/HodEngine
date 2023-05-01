#pragma once

#include "HodEngine/Application/GraphicApplications/GraphicApplication.h"

namespace hod::application
{
	/// @brief 
	class DesktopApplication : public GraphicApplication
	{
		_SingletonOverride(DesktopApplication)
	};
}

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.inl"
