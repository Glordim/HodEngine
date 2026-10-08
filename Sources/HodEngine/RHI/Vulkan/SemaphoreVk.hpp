#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Semaphore.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API SemaphoreVk : public Semaphore
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
		VkSemaphore					TakeVkSemaphore();

	private:

		VkSemaphore					_vkSempahore = VK_NULL_HANDLE;
	};
}
