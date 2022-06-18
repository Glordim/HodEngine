#pragma once

#include "HodEngine/Application/src/GraphicApplications/DesktopApplications/DesktopApplication.h"

namespace hod::application
{
	/// @brief 
	class LinuxApplication : public DesktopApplication
	{
	public:

		bool			Init(int argc, char** argv);
		bool			Run();
	};
}
