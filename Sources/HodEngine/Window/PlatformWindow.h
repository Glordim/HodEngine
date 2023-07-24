#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Window/DesktopWindow/Win32/Win32Window.h"
	#define PlatformWindow hod::window::Win32Window

#elif defined(PLATFORM_MAC)

	//#include "GraphicApplications/DesktopApplications/MacApplication.h"
	//#define PlatformApplication MacApplication

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Window/DesktopWindow/Wayland/WaylandWindow.h"
	#define PlatformWindow hod::window::WaylandWindow

#elif defined(PLATFORM_ANDROID)

	//#include "GraphicApplications/MobileApplications/AndroidApplication.h"
	//#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.h"
	//#define PlatformApplication IOSApplication

#endif
