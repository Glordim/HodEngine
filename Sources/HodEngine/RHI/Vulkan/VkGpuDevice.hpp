#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/GpuDevice.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	struct HOD_RHI_API VkGpuDevice : public GpuDevice
	{
		VkPhysicalDevice                 physicalDevice;
		uint32_t                         graphicsAndPresentQueueFamilyIndex;
		VkPhysicalDeviceProperties       deviceProperties;
		VkPhysicalDeviceMemoryProperties memProperties;
		uint32_t                         hostMemoryTypeIndex;
		uint32_t                         deviceMemoryTypeIndex;
	};
}
