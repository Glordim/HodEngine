#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Application/GraphicApplications/DesktopApplications/WindowsApplication.hpp"
	#define PlatformApplication hod::application::WindowsApplication

#elif defined(PLATFORM_MAC)

	#include "HodEngine/ApplicationGraphicApplications/DesktopApplications/MacApplication.hpp"
	#define PlatformApplication MacApplication

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Application/GraphicApplications/DesktopApplications/LinuxApplication.hpp"
	#define PlatformApplication hod::application::LinuxApplication

#elif defined(PLATFORM_ANDROID)

	#include "HodEngine/ApplicationGraphicApplications/MobileApplications/AndroidApplication.hpp"
	#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	#include "HodEngine/ApplicationGraphicApplications/MobileApplications/IOSApplication.hpp"
	#define PlatformApplication IOSApplication

#endif
