#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/GpuDevice.hpp"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		struct HOD_RENDERER_API VkGpuDevice : public GpuDevice
		{
			VkPhysicalDevice                 physicalDevice;
			uint32_t                         graphicsAndPresentQueueFamilyIndex;
			VkPhysicalDeviceProperties       deviceProperties;
			VkPhysicalDeviceMemoryProperties memProperties;
			uint32_t                         hostMemoryTypeIndex;
			uint32_t                         deviceMemoryTypeIndex;
		};
	}
}
