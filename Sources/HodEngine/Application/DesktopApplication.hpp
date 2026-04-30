#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplication.hpp"

namespace hod::inline application
{
	/// @brief
	class HOD_APPLICATION_API DesktopApplication : public GraphicApplication
	{
	public:
		bool Run(int argc, char** argv);
	};
}
