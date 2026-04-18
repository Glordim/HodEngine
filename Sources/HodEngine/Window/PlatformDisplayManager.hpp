#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
	#define PlatformDisplayManager hod::Win32DisplayManager

#elif defined(PLATFORM_MACOS)

	#include "HodEngine/Window/Desktop/MacOs/CocoaDisplayManager.hpp"
	#define PlatformDisplayManager hod::CocoaDisplayManager

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp"
	#define PlatformDisplayManager hod::WaylandDisplayManager

#elif defined(PLATFORM_ANDROID)

	#include "HodEngine/Window/Android/AndroidDisplayManager.hpp"
	#define PlatformDisplayManager hod::AndroidDisplayManager

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.hpp"
	//#define PlatformApplication IOSApplication

#endif
