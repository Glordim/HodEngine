#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Semaphore.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API VulkanSemaphore : public Semaphore
	{
	public:

									VulkanSemaphore();
									VulkanSemaphore(const VulkanSemaphore&) = delete;
									VulkanSemaphore(VulkanSemaphore&&) = delete;
									~VulkanSemaphore() override;

		VulkanSemaphore&				operator=(const VulkanSemaphore&) = delete;
		VulkanSemaphore&				operator=(VulkanSemaphore&&) = delete;

	public:

		VkSemaphore					GetVkSemaphore() const;
		VkSemaphore					TakeVkSemaphore();

	private:

		VkSemaphore					_vkSempahore = VK_NULL_HANDLE;
	};
}
