#pragma once

#include "../GpuDevice.h"

#include <vulkan/vulkan.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct VkGpuDevice : public GpuDevice
		{
			VkPhysicalDevice physicalDevice;
			uint32_t graphicsAndPresentQueueFamilyIndex;
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceMemoryProperties memProperties;
			uint32_t hostMemoryTypeIndex;
			uint32_t deviceMemoryTypeIndex;
		};
	}
}
