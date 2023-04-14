#pragma once

#include "HodEngine/Application/src/GraphicApplications/GraphicApplication.h"

namespace hod::application
{
	/// @brief 
	class DesktopApplication : public GraphicApplication
	{
		_SingletonOverride(DesktopApplication)
	};
}

#include "DesktopApplication.inl"
