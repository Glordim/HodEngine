#pragma once

#include "HodEngine/Application/src/GraphicApplications/DesktopApplications/DesktopApplication.h"

namespace HOD
{
	/// @brief 
	class MacApplication : public DesktopApplication
	{
	public:

		bool			Init(int argc, char** argv);
		bool			Run();
	};
}
