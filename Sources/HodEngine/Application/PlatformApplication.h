#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Application/GraphicApplications/DesktopApplications/WindowsApplication.h"
	#define PlatformApplication hod::application::WindowsApplication

#elif defined(PLATFORM_MAC)

	#include "HodEngine/ApplicationGraphicApplications/DesktopApplications/MacApplication.h"
	#define PlatformApplication MacApplication

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/ApplicationGraphicApplications/DesktopApplications/LinuxApplication.h"
	#define PlatformApplication LinuxApplication

#elif defined(PLATFORM_ANDROID)

	#include "HodEngine/ApplicationGraphicApplications/MobileApplications/AndroidApplication.h"
	#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	#include "HodEngine/ApplicationGraphicApplications/MobileApplications/IOSApplication.h"
	#define PlatformApplication IOSApplication

#endif
