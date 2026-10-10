#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanSampler.hpp"

#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	/// @brief
	/// @param createInfo
	VulkanSampler::VulkanSampler(const CreateInfo& createInfo)
	: Sampler(createInfo)
	{
	}

	/// @brief
	VulkanSampler::~VulkanSampler()
	{
		if (_vkSampler != VK_NULL_HANDLE)
		{
			VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();
			rhiDevice->DeferDestroy(_vkSampler);
		}
	}

	/// @brief
	/// @return
	bool VulkanSampler::Build()
	{
		const CreateInfo& createInfo = GetCreateInfo();

		VkSamplerAddressMode addressMode = createInfo._wrapMode == WrapMode::Clamp ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkFilter             filter = createInfo._filterMode == FilterMode::Linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		VkSamplerMipmapMode  mipmapMode = createInfo._filterMode == FilterMode::Linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = mipmapMode;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();
		if (vkCreateSampler(rhiDevice->GetVkDevice(), &samplerInfo, nullptr, &_vkSampler) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Failed to create texture sampler!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	VkSampler VulkanSampler::GetVkSampler() const
	{
		return _vkSampler;
	}
}
