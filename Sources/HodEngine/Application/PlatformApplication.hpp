#pragma once

#if defined(HOD_SERVER)
	#include "HodEngine/Application/Application.hpp"
	#define PlatformApplication hod::Application
#else
	#if defined(PLATFORM_WINDOWS)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/Win32/WindowsApplication.hpp"
		#define PlatformApplication hod::WindowsApplication
	#elif defined(PLATFORM_MACOS)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/MacOs/MacOsApplication.hpp"
		#define PlatformApplication hod::MacOsApplication
	#elif defined(PLATFORM_LINUX)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/Linux/LinuxApplication.hpp"
		#define PlatformApplication hod::LinuxApplication
	#elif defined(PLATFORM_ANDROID)
		#include "HodEngine/Application/GraphicApplications/MobileApplications/Android/AndroidApplication.hpp"
		#define PlatformApplication hod::AndroidApplication
	#elif defined(PLATFORM_IOS)
		#include "HodEngine/Application/GraphicApplications/MobileApplications/IOS/IOSApplication.hpp"
		#define PlatformApplication hod::IOSApplication
	#endif
#endif
