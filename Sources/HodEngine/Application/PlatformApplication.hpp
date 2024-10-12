#pragma once

#if defined(HOD_SERVER)
	#include "HodEngine/Application/Application.hpp"
	#define PlatformApplication hod::application::Application
#else
	#if defined(PLATFORM_WINDOWS)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/WindowsApplication.hpp"
		#define PlatformApplication hod::application::WindowsApplication
	#elif defined(PLATFORM_MACOS)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/MacOsApplication.hpp"
		#define PlatformApplication hod::application::MacOsApplication
	#elif defined(PLATFORM_LINUX)
		#include "HodEngine/Application/GraphicApplications/DesktopApplications/LinuxApplication.hpp"
		#define PlatformApplication hod::application::LinuxApplication
	#elif defined(PLATFORM_ANDROID)
		#include "HodEngine/Application/GraphicApplications/MobileApplications/Android/AndroidApplication.hpp"
		#define PlatformApplication hod::application::AndroidApplication
	#elif defined(PLATFORM_IOS)
		#include "HodEngine/Application/GraphicApplications/MobileApplications/IOS/IOSApplication.hpp"
		#define PlatformApplication hod::application::IOSApplication
	#endif
#endif
