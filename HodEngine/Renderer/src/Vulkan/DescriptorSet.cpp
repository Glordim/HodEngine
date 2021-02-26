#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"

#include "VkTexture.h"

#include "RendererVulkan.h"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DescriptorSet::DescriptorSet()
		{
			_descriptorSetLayout = nullptr;
			_descriptorSet = VK_NULL_HANDLE;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DescriptorSet::~DescriptorSet()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			size_t uboCount = _uboBuffers.size();
			for (size_t i = 0; i < uboCount; ++i)
			{
				const VkBuffer buffer = _uboBuffers[i];
				const VkDeviceMemory bufferMemory = _uboBufferMemories[i];

				if (buffer != VK_NULL_HANDLE)
					vkDestroyBuffer(renderer->GetVkDevice(), buffer, nullptr);
				if (bufferMemory != VK_NULL_HANDLE)
					vkFreeMemory(renderer->GetVkDevice(), bufferMemory, nullptr);
			}

			if (_descriptorSet != VK_NULL_HANDLE)
				vkFreeDescriptorSets(renderer->GetVkDevice(), renderer->GetDescriptorPool(), 1, &_descriptorSet);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool DescriptorSet::SetLayout(const DescriptorSetLayout* layout)
		{
			_descriptorSetLayout = layout;

			VkDescriptorSetLayout vkLayout = _descriptorSetLayout->GetDescriptorSetLayout();

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = renderer->GetDescriptorPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkLayout;

			if (vkAllocateDescriptorSets(renderer->GetVkDevice(), &allocInfo, &_descriptorSet) != VK_SUCCESS)
			{
				fprintf(stderr, "Vulkan: Unable to allocate descriptor sets!\n");
				return false;
			}

			const std::vector<DescriptorSetLayout::BlockUbo>& ubos = _descriptorSetLayout->GetUboBlocks();
			size_t uboCount = ubos.size();

			_uboBuffers.resize(uboCount, VK_NULL_HANDLE);
			_uboBufferMemories.resize(uboCount, VK_NULL_HANDLE);

			for (size_t i = 0; i < uboCount; ++i)
			{
				const DescriptorSetLayout::BlockUbo& ubo = ubos[i];

				if (renderer->CreateBuffer(ubo._rootMember._size * ubo._rootMember._count, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uboBuffers.data() + i, _uboBufferMemories.data() + i) == false)
				{
					return false;
				}
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const DescriptorSetLayout* DescriptorSet::GetLayout() const
		{
			return _descriptorSetLayout;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkDescriptorSet DescriptorSet::GetDescriptorSet() const
		{
			return _descriptorSet;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DescriptorSet::SetUboValue(const std::string& memberName, const void* value, size_t valueSize)
		{
			std::string varIdentifier;

			int index = 1;
			std::string target;

			size_t len = memberName.find_first_of(".[");
			if (len == std::string::npos)
			{
				target = memberName;
				varIdentifier = "";
			}
			else
			{
				target = memberName.substr(0, len);
				varIdentifier = memberName.substr(len);
			}

			const std::vector<DescriptorSetLayout::BlockUbo>& ubos = _descriptorSetLayout->GetUboBlocks();
			size_t uboCount = ubos.size();

			for (size_t i = 0; i < uboCount; ++i)
			{
				const DescriptorSetLayout::BlockUbo& ubo = ubos[i];

				if (ubo._name == target)
				{
					size_t offset = 0;
					const DescriptorSetLayout::BlockUbo::Member* member = &ubo._rootMember;

					while (varIdentifier.empty() == false)
					{
						if (varIdentifier[0] == '.')
						{
							std::string subMemberName;

							size_t len = varIdentifier.find_first_of(".[", 1);

							if (len == std::string::npos)
							{
								subMemberName = varIdentifier.substr(1);
								varIdentifier = "";
							}
							else
							{
								subMemberName = varIdentifier.substr(1, len - 1);
								varIdentifier = varIdentifier.substr(len);
							}

							auto it = member->_childsMap.find(subMemberName);

							member = &it->second;

							offset += member->_offset;
						}
						else if (varIdentifier[0] == '[')
						{
							size_t len = varIdentifier.find_first_of("]", 1);

							std::string indexStr = varIdentifier.substr(1, len - 1);
							int index = std::atoi(indexStr.c_str());

							offset += member->_size * index;

							varIdentifier = varIdentifier.substr(len + 1);
						}
					}

					if (member->_size != valueSize)
					{
						fprintf(stderr, "SetUboValue: member->size != valueSize !\n");
					}

					RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

					void* data = nullptr;

					if (vkMapMemory(renderer->GetVkDevice(), _uboBufferMemories[i], 0, ubo._rootMember._size * ubo._rootMember._count, 0, &data) != VK_SUCCESS)
					{
						fprintf(stderr, "Vulkan: Unable to map ubo buffer memory!\n");
						return;
					}
					memcpy(((char*)data) + offset, value, valueSize);
					vkUnmapMemory(renderer->GetVkDevice(), _uboBufferMemories[i]);

					VkDescriptorBufferInfo bufferInfo = {};
					bufferInfo.buffer = _uboBuffers[i];
					bufferInfo.offset = 0;
					bufferInfo.range = ubo._rootMember._size * ubo._rootMember._count;

					VkWriteDescriptorSet descriptorWrite = {};
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = _descriptorSet;
					descriptorWrite.dstBinding = ubo._binding;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = 1;
					descriptorWrite.pBufferInfo = &bufferInfo;
					descriptorWrite.pImageInfo = nullptr;
					descriptorWrite.pTexelBufferView = nullptr;
					descriptorWrite.pNext = nullptr;

					vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
				}
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DescriptorSet::SetTexture(const std::string& name, const VkTexture* textureSampler)
		{
			const std::vector<DescriptorSetLayout::BlockTexture>& textures = _descriptorSetLayout->GetTextureBlocks();
			size_t textureCount = textures.size();

			for (size_t i = 0; i < textureCount; ++i)
			{
				const DescriptorSetLayout::BlockTexture& texture = textures[i];

				if (texture._name == name)
				{
					RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

					VkDescriptorImageInfo imageInfo = {};
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = textureSampler->GetTextureImageView();
					imageInfo.sampler = textureSampler->GetTextureSampler();

					VkWriteDescriptorSet descriptorWrite = {};
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = _descriptorSet;
					descriptorWrite.dstBinding = texture._binding;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrite.descriptorCount = 1;
					descriptorWrite.pBufferInfo = nullptr;
					descriptorWrite.pImageInfo = &imageInfo;
					descriptorWrite.pTexelBufferView = nullptr;
					descriptorWrite.pNext = nullptr;

					vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);

					break;
				}
			}
		}
	}
}
