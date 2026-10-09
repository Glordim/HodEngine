#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanFence.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	/// @brief 
	VulkanFence::VulkanFence()
		: Fence()
	{
		VkDevice device = VulkanRhiDevice::GetInstance()->GetVkDevice();

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
	VulkanFence::~VulkanFence()
	{
		if (_vkFence != VK_NULL_HANDLE)
		{
			VkDevice device = VulkanRhiDevice::GetInstance()->GetVkDevice();
			vkDestroyFence(device, _vkFence, nullptr);
		}
	}

	/// @brief 
	/// @return 
	VkFence VulkanFence::GetVkFence() const
	{
		return _vkFence;
	}

	/// @brief 
	/// @return 
	bool VulkanFence::Reset()
	{
		VkDevice device = VulkanRhiDevice::GetInstance()->GetVkDevice();
		if (vkResetFences(device, 1, &_vkFence) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to reset fence!");
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	bool VulkanFence::Wait()
	{
		VkDevice device = VulkanRhiDevice::GetInstance()->GetVkDevice();
		if (vkWaitForFences(device, 1, &_vkFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to wait fence");
			return false;
		}
		return true;
	}
}
