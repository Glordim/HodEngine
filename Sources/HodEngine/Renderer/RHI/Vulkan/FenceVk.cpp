#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/FenceVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <VulkanMemoryAllocator/vk_mem_alloc.h>

namespace hod::renderer
{
	/// @brief 
	FenceVk::FenceVk()
		: Fence()
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		fenceCreateInfo.pNext = nullptr;

		if (vkCreateFence(device, &fenceCreateInfo, nullptr, &_vkFence) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create fence!");
			return;
		}
	}

	/// @brief 
	FenceVk::~FenceVk()
	{
		if (_vkFence != VK_NULL_HANDLE)
		{
			VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();
			vkDestroyFence(device, _vkFence, nullptr);
		}
	}

	/// @brief 
	/// @return 
	VkFence FenceVk::GetVkFence() const
	{
		return _vkFence;
	}

	/// @brief 
	/// @return 
	bool FenceVk::Reset()
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();
		if (vkResetFences(device, 1, &_vkFence) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to reset fence!");
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	bool FenceVk::Wait()
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();
		if (vkWaitForFences(device, 1, &_vkFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to wait fence");
			return false;
		}
		return true;
	}
}
