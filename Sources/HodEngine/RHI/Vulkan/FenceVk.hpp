#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Fence.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API FenceVk : public Fence
	{
	public:

						FenceVk();
						FenceVk(const FenceVk&) = delete;
						FenceVk(FenceVk&&) = delete;
						~FenceVk() override;

		FenceVk&		operator=(const FenceVk&) = delete;
		FenceVk&		operator=(FenceVk&&) = delete;

	public:

		bool			Reset() override;
		bool			Wait() override;

		VkFence			GetVkFence() const;

	private:

		VkFence			_vkFence = VK_NULL_HANDLE;
	};
}
