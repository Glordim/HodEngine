#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Fence.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API VulkanFence : public Fence
	{
	public:

						VulkanFence();
						VulkanFence(const VulkanFence&) = delete;
						VulkanFence(VulkanFence&&) = delete;
						~VulkanFence() override;

		VulkanFence&		operator=(const VulkanFence&) = delete;
		VulkanFence&		operator=(VulkanFence&&) = delete;

	public:

		bool			Reset() override;
		bool			Wait() override;

		VkFence			GetVkFence() const;

	private:

		VkFence			_vkFence = VK_NULL_HANDLE;
	};
}
