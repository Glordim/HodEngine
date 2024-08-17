#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplications/GraphicApplication.hpp"

namespace hod::application
{
	/// @brief 
	class HOD_APPLICATION_API DesktopApplication : public GraphicApplication
	{
		_SingletonOverride(DesktopApplication)
	};
}

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.inl"
