#pragma once

#include "HodEngine/Application/GraphicApplications/GraphicApplication.h"

namespace hod::application
{
	/// @brief 
	class AndroidApplication : public GraphicApplication
	{
	public:

		bool			Init(int argc, char** argv);
		bool			Run();
	};
}
