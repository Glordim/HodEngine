#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.h"
	#define PlatformDisplayManager hod::window::Win32DisplayManager

#elif defined(PLATFORM_MAC)

	//#include "GraphicApplications/DesktopApplications/MacApplication.h"
	//#define PlatformApplication MacApplication

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.h"
	#define PlatformDisplayManager hod::window::WaylandDisplayManager

#elif defined(PLATFORM_ANDROID)

	//#include "GraphicApplications/MobileApplications/AndroidApplication.h"
	//#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.h"
	//#define PlatformApplication IOSApplication

#endif