#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanBindGroup.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/Vulkan/VulkanBuffer.hpp"
#include "HodEngine/RHI/Vulkan/VulkanGraphicsPipeline.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"
#include "HodEngine/RHI/Vulkan/VulkanSampler.hpp"
#include "HodEngine/RHI/Vulkan/VulkanTexture.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <algorithm>

namespace hod::inline rhi
{
	/// @brief
	/// @param type
	/// @return
	VkDescriptorType VulkanBindGroup::TextureTypeToVkDescriptorType(ShaderSetDescriptor::BlockTexture::Type type)
	{
		switch (type)
		{
			case ShaderSetDescriptor::BlockTexture::Type::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;

			case ShaderSetDescriptor::BlockTexture::Type::Texture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

			case ShaderSetDescriptor::BlockTexture::Type::Combined: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	/// @brief
	VulkanBindGroup::~VulkanBindGroup()
	{
		if (_descriptorSet != VK_NULL_HANDLE)
		{
			VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();
			rhiDevice->DeferDestroy(_descriptorSet);
		}
	}

	/// @brief
	/// @param graphicsPipeline
	/// @param set
	/// @param uniformBuffers one per uniform block of the set
	/// @param uniformBufferCount
	/// @param textureBindings one per texture block of the set
	/// @param textureBindingCount
	/// @return
	bool VulkanBindGroup::Build(const VulkanGraphicsPipeline& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                            const TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		const std::map<uint32_t, ShaderSetDescriptor*>& setDescriptors = graphicsPipeline.GetSetDescriptors();
		auto                                            it = setDescriptors.find(set);
		if (it == setDescriptors.end())
		{
			OUTPUT_ERROR("Vulkan: BindGroup, the pipeline has no set {}", set);
			return false;
		}

		const Vector<ShaderSetDescriptor::BlockUbo>&     uboBlocks = it->second->GetUboBlocks();
		const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = it->second->GetTextureBlocks();
		if (uniformBufferCount != uboBlocks.Size() || textureBindingCount != textureBlocks.Size())
		{
			OUTPUT_ERROR("Vulkan: BindGroup, set {} expects {} uniform buffers and {} textures, got {} and {}", set, uboBlocks.Size(), textureBlocks.Size(), uniformBufferCount,
			             textureBindingCount);
			return false;
		}

		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		VkDescriptorSetLayout descriptorSetLayout = graphicsPipeline.GetDescriptorSetLayout(set);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = rhiDevice->GetDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		if (vkAllocateDescriptorSets(rhiDevice->GetVkDevice(), &allocInfo, &_descriptorSet) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: BindGroup, unable to allocate the descriptor set!");
			return false;
		}

		for (uint32_t i = 0; i < uniformBufferCount; ++i)
		{
			const ShaderSetDescriptor::BlockUbo& ubo = uboBlocks[i];
			if (uniformBuffers[i] == nullptr)
			{
				OUTPUT_ERROR("Vulkan: BindGroup, no uniform buffer given for \"{}\"", ubo._name);
				return false;
			}

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = static_cast<const VulkanBuffer*>(uniformBuffers[i])->GetVkBuffer();
			bufferInfo.offset = 0; // the offset is dynamic, given when the set is bound
			bufferInfo.range = ubo._rootMember._size * ubo._rootMember._count;

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSet;
			descriptorWrite.dstBinding = ubo._binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(rhiDevice->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		const VulkanTexture* fallbackTexture = static_cast<const VulkanTexture*>(rhiDevice->GetFallbackTexture());
		for (uint32_t i = 0; i < textureBindingCount; ++i)
		{
			const ShaderSetDescriptor::BlockTexture& textureBlock = textureBlocks[i];
			const TextureBinding&                    textureBinding = textureBindings[i];

			const VulkanTexture* texture = textureBinding._texture != nullptr ? static_cast<const VulkanTexture*>(textureBinding._texture) : fallbackTexture;

			const Sampler* sampler = textureBinding._sampler != nullptr ? textureBinding._sampler : texture->GetDefaultSampler();
			if (sampler == nullptr)
			{
				sampler = rhiDevice->GetSampler(Sampler::CreateInfo());
			}
			VkSampler vkSampler = static_cast<const VulkanSampler*>(sampler)->GetVkSampler();

			VkDescriptorImageInfo imageInfo = {};
			if (textureBlock._type == ShaderSetDescriptor::BlockTexture::Type::Sampler)
			{
				imageInfo.sampler = vkSampler;
			}
			else if (textureBlock._type == ShaderSetDescriptor::BlockTexture::Type::Texture)
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture->GetTextureImageView();
			}
			else
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture->GetTextureImageView();
				imageInfo.sampler = vkSampler;
			}

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSet;
			descriptorWrite.dstBinding = textureBlock._binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = TextureTypeToVkDescriptorType(textureBlock._type);
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(rhiDevice->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		_uniformBufferOrder.Resize(uniformBufferCount);
		for (uint32_t i = 0; i < uniformBufferCount; ++i)
		{
			_uniformBufferOrder[i] = i;
		}
		std::sort(_uniformBufferOrder.Data(), _uniformBufferOrder.Data() + _uniformBufferOrder.Size(),
		          [&uboBlocks](uint32_t left, uint32_t right) { return uboBlocks[left]._binding < uboBlocks[right]._binding; });

		return true;
	}

	/// @brief
	/// @return
	VkDescriptorSet VulkanBindGroup::GetDescriptorSet() const
	{
		return _descriptorSet;
	}

	/// @brief
	/// @return
	const Vector<uint32_t>& VulkanBindGroup::GetUniformBufferOrder() const
	{
		return _uniformBufferOrder;
	}
}
