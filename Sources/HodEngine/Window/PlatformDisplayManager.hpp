#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
	#define PlatformDisplayManager hod::window::Win32DisplayManager

#elif defined(PLATFORM_MACOS)

	#include "HodEngine/Window/Desktop/MacOs/CocoaDisplayManager.hpp"
	#define PlatformDisplayManager hod::window::CocoaDisplayManager

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp"
	#define PlatformDisplayManager hod::window::WaylandDisplayManager

#elif defined(PLATFORM_ANDROID)

	//#include "GraphicApplications/MobileApplications/AndroidApplication.hpp"
	//#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.hpp"
	//#define PlatformApplication IOSApplication

#endif
