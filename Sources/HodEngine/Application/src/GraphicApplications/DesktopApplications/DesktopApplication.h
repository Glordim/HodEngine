#pragma once

#include "HodEngine/Application/src/GraphicApplications/GraphicApplication.h"

namespace HOD
{
	/// @brief 
	class DesktopApplication : public GraphicApplication
	{
		_Singleton(DesktopApplication)

						DesktopApplication() = default;

	public:

	};
}

#include "DesktopApplication.inl"
