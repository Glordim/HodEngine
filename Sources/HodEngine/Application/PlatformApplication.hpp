#pragma once

#if defined(HOD_SERVER)
	#include "HodEngine/Application/Application.hpp"
	#define PlatformApplication hod::application::Application
#else
	#if defined(PLATFORM_WINDOWS)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/WindowsApplication.hpp"
		#define PlatformApplication hod::application::WindowsApplication
	#elif defined(PLATFORM_MAC)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/MacApplication.hpp"
		#define PlatformApplication MacApplication
	#elif defined(PLATFORM_LINUX)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/LinuxApplication.hpp"
		#define PlatformApplication hod::application::LinuxApplication
	#elif defined(PLATFORM_ANDROID)
		#include "HodEngine/Application/GraphicApplications/MobileApplications/AndroidApplication.hpp"
		#define PlatformApplication AndroidApplication
	#elif defined(PLATFORM_IOS)
		#include "HodEngine/Application/GraphicApplications/MobileApplications/IOSApplication.hpp"
		#define PlatformApplication IOSApplication
	#endif
#endif
