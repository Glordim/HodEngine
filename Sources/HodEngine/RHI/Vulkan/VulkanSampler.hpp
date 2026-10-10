#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Sampler.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API VulkanSampler : public Sampler
	{
	public:
		VulkanSampler(const CreateInfo& createInfo);
		~VulkanSampler() override;

		bool Build();

		VkSampler GetVkSampler() const;

	private:
		VkSampler _vkSampler = VK_NULL_HANDLE;
	};
}
