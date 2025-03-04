#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/SemaphoreVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <vk_mem_alloc.h>

namespace hod::renderer
{
	/// @brief 
	SemaphoreVk::SemaphoreVk()
		: Semaphore()
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();

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
			VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();
			vkDestroySemaphore(device, _vkSempahore, nullptr);
		}
	}

	/// @brief 
	/// @return 
	VkSemaphore SemaphoreVk::GetVkSemaphore() const
	{
		return _vkSempahore;
	}
}
