#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.h"
	#define PlatformRenderer hod::renderer::RendererVulkan

#elif defined(PLATFORM_MAC)

	//#include "GraphicApplications/DesktopApplications/MacApplication.h"
	//#define PlatformApplication MacApplication

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.h"
	#define PlatformRenderer hod::renderer::RendererVulkan

#elif defined(PLATFORM_ANDROID)

	//#include "GraphicApplications/MobileApplications/AndroidApplication.h"
	//#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.h"
	//#define PlatformApplication IOSApplication

#endif
