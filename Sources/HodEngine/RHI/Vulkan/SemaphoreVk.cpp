#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/SemaphoreVk.hpp"
#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	/// @brief 
	SemaphoreVk::SemaphoreVk()
		: Semaphore()
	{
		VkDevice device = RhiDeviceVulkan::GetInstance()->GetVkDevice();

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_vkSempahore) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create Semaphore!");
			return;
		}
	}

	/// @brief
	SemaphoreVk::~SemaphoreVk()
	{
		if (_vkSempahore != VK_NULL_HANDLE)
		{
			RhiDeviceVulkan::GetInstance()->DeferDestroy(_vkSempahore);
		}
	}

	/// @brief
	/// @return
	VkSemaphore SemaphoreVk::GetVkSemaphore() const
	{
		return _vkSempahore;
	}

	/// @brief Transfers ownership of the VkSemaphore handle to the caller.
	/// The destructor will not destroy it.
	/// @return
	VkSemaphore SemaphoreVk::TakeVkSemaphore()
	{
		VkSemaphore handle = _vkSempahore;
		_vkSempahore = VK_NULL_HANDLE;
		return handle;
	}
}
