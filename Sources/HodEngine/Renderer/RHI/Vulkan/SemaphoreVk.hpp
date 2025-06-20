#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include <vulkan/vulkan.h>
#include <VulkanMemoryAllocator/vk_mem_alloc.h>

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API SemaphoreVk : public Semaphore
	{
	public:

									SemaphoreVk();
									SemaphoreVk(const SemaphoreVk&) = delete;
									SemaphoreVk(SemaphoreVk&&) = delete;
									~SemaphoreVk() override;

		SemaphoreVk&				operator=(const SemaphoreVk&) = delete;
		SemaphoreVk&				operator=(SemaphoreVk&&) = delete;

	public:

		VkSemaphore					GetVkSemaphore() const;

	private:

		VkSemaphore					_vkSempahore = VK_NULL_HANDLE;
	};
}
