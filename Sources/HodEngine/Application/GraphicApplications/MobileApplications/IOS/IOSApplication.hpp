#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplications/GraphicApplication.hpp"

namespace hod::application
{
	/// @brief
	class HOD_APPLICATION_API IOSApplication : public GraphicApplication
	{
	public:
		bool Init(int argc, char** argv);
		bool Run();
	};
}
