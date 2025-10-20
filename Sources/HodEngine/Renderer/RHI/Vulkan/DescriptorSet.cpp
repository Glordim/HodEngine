#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSet.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>

namespace hod::renderer
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
			DefaultAllocator::GetInstance().Delete(_uboBuffers[i]);
		}

		if (_descriptorSet != VK_NULL_HANDLE)
		{
			vkFreeDescriptorSets(renderer->GetVkDevice(), renderer->GetDescriptorPool(), 1, &_descriptorSet);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool DescriptorSet::SetLayout(const ShaderSetDescriptorVk* layout)
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
			OUTPUT_ERROR("Vulkan: Unable to allocate descriptor sets!");
			return false;
		}

		const Vector<ShaderSetDescriptorVk::BlockUbo>& ubos = _descriptorSetLayout->GetUboBlocks();
		size_t                                         uboCount = ubos.size();

		for (BufferVk* buffer : _uboBuffers)
		{
			DefaultAllocator::GetInstance().Delete(buffer);
		}

		_uboBuffers.resize(uboCount, nullptr);

		for (size_t i = 0; i < uboCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockUbo& ubo = ubos[i];

			uint32_t size = static_cast<uint32_t>(ubo._rootMember._size * ubo._rootMember._count);

			_uboBuffers[i] = static_cast<BufferVk*>(Renderer::GetInstance()->CreateBuffer(Buffer::Usage::Uniform, size));
			if (_uboBuffers[i] == nullptr)
			{
				return false;
			}

			void* data = _uboBuffers[i]->Lock();
			if (data != nullptr)
			{
				memset(data, 0, size);
				_uboBuffers[i]->Unlock();
			}

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = _uboBuffers[i]->GetVkBuffer();
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

		const VkTexture* defaultTexture = static_cast<const VkTexture*>(Renderer::GetInstance()->GetDefaultWhiteTexture());

		const Vector<ShaderSetDescriptorVk::BlockTexture>& textures = _descriptorSetLayout->GetTextureBlocks();
		size_t                                             textureCount = textures.size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockTexture& texture = textures[i];

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkDescriptorImageInfo imageInfo = {};
			if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Sampler)
			{
				imageInfo.sampler = defaultTexture->GetTextureSampler();
			}
			else if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Texture)
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = defaultTexture->GetTextureImageView();
			}
			else
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = defaultTexture->GetTextureImageView();
				imageInfo.sampler = defaultTexture->GetTextureSampler();
			}

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSet;
			descriptorWrite.dstBinding = texture._binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(texture._type);
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = nullptr;
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pTexelBufferView = nullptr;
			descriptorWrite.pNext = nullptr;

			vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const ShaderSetDescriptorVk* DescriptorSet::GetLayout() const
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
	void DescriptorSet::SetUboValue(const String& memberName, const void* value, size_t valueSize)
	{
		String varIdentifier;
		String target;

		size_t len = memberName.find_first_of(".[");
		if (len == String::npos)
		{
			target = memberName;
			varIdentifier = "";
		}
		else
		{
			target = memberName.substr(0, len);
			varIdentifier = memberName.substr(len);
		}

		const Vector<ShaderSetDescriptorVk::BlockUbo>& ubos = _descriptorSetLayout->GetUboBlocks();
		size_t                                         uboCount = ubos.size();

		for (size_t i = 0; i < uboCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockUbo& ubo = ubos[i];

			if (ubo._name == target)
			{
				size_t                                         offset = 0;
				const ShaderSetDescriptorVk::BlockUbo::Member* member = &ubo._rootMember;

				while (varIdentifier.empty() == false)
				{
					if (varIdentifier[0] == '.')
					{
						String subMemberName;

						size_t len = varIdentifier.find_first_of(".[", 1);

						if (len == String::npos)
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
						if (it == member->_childsMap.end())
						{
							return;
						}

						member = &it->second;

						offset += member->_offset;
					}
					else if (varIdentifier[0] == '[')
					{
						size_t len = varIdentifier.find_first_of("]", 1);

						String indexStr = varIdentifier.substr(1, len - 1);
						int    index = std::atoi(indexStr.c_str());

						offset += member->_size * index;

						varIdentifier = varIdentifier.substr(len + 1);
					}
				}

				if (member->_size != valueSize)
				{
					OUTPUT_ERROR("SetUboValue: member->size != valueSize !");
				}

				RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

				void* data = _uboBuffers[i]->Lock();
				if (data != nullptr)
				{
					memcpy(((char*)data) + offset, value, valueSize);
					_uboBuffers[i]->Unlock();
				}

				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = _uboBuffers[i]->GetVkBuffer();
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
	void DescriptorSet::SetTexture(const String& name, const VkTexture* textureSampler)
	{
		const Vector<ShaderSetDescriptorVk::BlockTexture>& textures = _descriptorSetLayout->GetTextureBlocks();
		size_t                                             textureCount = textures.size();

		bool founded = false;
		for (size_t i = 0; i < textureCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockTexture& texture = textures[i];

			if (texture._name == name)
			{
				founded = true;
				break;
			}
		}

		if (founded == false)
		{
			return;
		}

		for (size_t i = 0; i < textureCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockTexture& texture = textures[i];

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkDescriptorImageInfo imageInfo = {};
			if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Sampler)
			{
				imageInfo.sampler = textureSampler->GetTextureSampler();
			}
			else if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Texture)
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textureSampler->GetTextureImageView();
			}
			else
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textureSampler->GetTextureImageView();
				imageInfo.sampler = textureSampler->GetTextureSampler();
			}

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSet;
			descriptorWrite.dstBinding = texture._binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(texture._type);
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = nullptr;
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pTexelBufferView = nullptr;
			descriptorWrite.pNext = nullptr;

			vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}
}
