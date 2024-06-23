#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
	#define PlatformRenderer hod::renderer::RendererVulkan

#elif defined(PLATFORM_MACOS)

	#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
	#define PlatformRenderer hod::renderer::RendererMetal

#elif defined(PLATFORM_LINUX)

	#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
	#define PlatformRenderer hod::renderer::RendererVulkan

#elif defined(PLATFORM_ANDROID)

	//#include "GraphicApplications/MobileApplications/AndroidApplication.hpp"
	//#define PlatformApplication AndroidApplication

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.hpp"
	//#define PlatformApplication IOSApplication

#endif
