#pragma once

#include "HodEngine/Application/src/GraphicApplications/GraphicApplication.h"

namespace HOD
{
	/// @brief 
	class IOSApplication : public GraphicApplication
	{
	public:

		bool			Init(int argc, char** argv);
		bool			Run();
	};
}
