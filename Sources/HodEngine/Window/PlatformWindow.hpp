#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"
	#define PlatformWindow hod::window::Win32Window

#elif defined(PLATFORM_MAC)

	#include "HodEngine/Window/Desktop/Windows/MacOs/MacOsWindow.hpp"
	#define PlatformWindow hod::window::MacOsWindow

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandWindow.hpp"
	#define PlatformWindow hod::window::WaylandWindow

#elif defined(PLATFORM_ANDROID)

	//#include "GraphicApplications/MobileApplications/AndroidApplication.hpp"
	//#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.hpp"
	//#define PlatformApplication IOSApplication

#endif
