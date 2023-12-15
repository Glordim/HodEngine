#pragma once

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"

namespace hod::application
{
	/// @brief 
	class MacApplication : public DesktopApplication
	{
	public:

		bool			Init(int argc, char** argv);
		bool			Run();
	};
}
