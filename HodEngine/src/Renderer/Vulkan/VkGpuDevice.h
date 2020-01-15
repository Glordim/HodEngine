#ifndef __VK_GPU_DEVICE_HPP__
#define __VK_GPU_DEVICE_HPP__

#include "../GpuDevice.h"

#include <vulkan/vulkan.h>

struct VkGpuDevice : public GpuDevice
{
    VkPhysicalDevice physicalDevice;
    uint32_t graphicsAndPresentQueueFamilyIndex;
    VkPhysicalDeviceMemoryProperties memProperties;
    uint32_t hostMemoryTypeIndex;
    uint32_t deviceMemoryTypeIndex;
};

#endif
