#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSet.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	DescriptorSet::DescriptorSet()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	DescriptorSet::~DescriptorSet()
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		for (PerFrameData& frameData : _perFrameData)
		{
			for (BufferVk* buffer : frameData.uboBuffers)
			{
				renderer->DeferDestroy(buffer->GetVkBuffer(), buffer->GetVmaAllocation());
				buffer->Detach();
				DefaultAllocator::GetInstance().Delete(buffer);
			}

			if (frameData.descriptorSet != VK_NULL_HANDLE)
			{
				renderer->DeferDestroy(frameData.descriptorSet);
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool DescriptorSet::SetLayout(const ShaderSetDescriptorVk* layout)
	{
		_descriptorSetLayout = layout;

		VkDescriptorSetLayout vkLayout      = _descriptorSetLayout->GetDescriptorSetLayout();
		RendererVulkan*       renderer      = (RendererVulkan*)Renderer::GetInstance();
		uint32_t              frameInFlight = Renderer::GetInstance()->GetFrameInFlightCount();

		_perFrameData.Resize(frameInFlight);

		const VkTexture* defaultTexture = static_cast<const VkTexture*>(Renderer::GetInstance()->GetDefaultWhiteTexture());

		const Vector<ShaderSetDescriptorVk::BlockUbo>&     ubos         = _descriptorSetLayout->GetUboBlocks();
		const Vector<ShaderSetDescriptorVk::BlockTexture>& textures     = _descriptorSetLayout->GetTextureBlocks();
		size_t                                              uboCount     = ubos.Size();
		size_t                                              textureCount = textures.Size();

		_uboData.Resize(uboCount);
		for (size_t i = 0; i < uboCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockUbo& ubo  = ubos[i];
			size_t                                  size = ubo._rootMember._size * ubo._rootMember._count;
			_uboData[i].Resize(size, 0);
		}

		_currentTextures.Resize(textureCount, defaultTexture);

		for (uint32_t frameIndex = 0; frameIndex < frameInFlight; ++frameIndex)
		{
			PerFrameData& frameData = _perFrameData[frameIndex];

			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool     = renderer->GetDescriptorPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts        = &vkLayout;

			if (vkAllocateDescriptorSets(renderer->GetVkDevice(), &allocInfo, &frameData.descriptorSet) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to allocate descriptor sets!");
				return false;
			}

			frameData.uboBuffers.Resize(uboCount, nullptr);

			for (size_t i = 0; i < uboCount; ++i)
			{
				const ShaderSetDescriptorVk::BlockUbo& ubo  = ubos[i];
				uint32_t                               size = static_cast<uint32_t>(ubo._rootMember._size * ubo._rootMember._count);

				frameData.uboBuffers[i] = static_cast<BufferVk*>(Renderer::GetInstance()->CreateBuffer(Buffer::Usage::Uniform, size));
				if (frameData.uboBuffers[i] == nullptr)
				{
					return false;
				}

				void* data = frameData.uboBuffers[i]->Lock();
				if (data != nullptr)
				{
					memset(data, 0, size);
					frameData.uboBuffers[i]->Unlock();
				}

				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = frameData.uboBuffers[i]->GetVkBuffer();
				bufferInfo.offset = 0;
				bufferInfo.range  = ubo._rootMember._size * ubo._rootMember._count;

				VkWriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet          = frameData.descriptorSet;
				descriptorWrite.dstBinding      = ubo._binding;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo     = &bufferInfo;

				vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
			}

			for (size_t i = 0; i < textureCount; ++i)
			{
				const ShaderSetDescriptorVk::BlockTexture& texture = textures[i];

				VkDescriptorImageInfo imageInfo = {};
				if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Sampler)
				{
					imageInfo.sampler = defaultTexture->GetTextureSampler();
				}
				else if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Texture)
				{
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView   = defaultTexture->GetTextureImageView();
				}
				else
				{
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView   = defaultTexture->GetTextureImageView();
					imageInfo.sampler     = defaultTexture->GetTextureSampler();
				}

				VkWriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet          = frameData.descriptorSet;
				descriptorWrite.dstBinding      = texture._binding;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType  = ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(texture._type);
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pImageInfo      = &imageInfo;

				vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkDescriptorSet DescriptorSet::GetDescriptorSet()
	{
		uint32_t      frameIndex = Renderer::GetInstance()->GetFrameIndex();
		PerFrameData& frameData  = _perFrameData[frameIndex];

		if (frameData.uboDirty)
		{
			FlushUboUpdates(frameIndex);
		}

		if (frameData.textureDirty)
		{
			FlushTextureUpdates(frameIndex);
		}

		return frameData.descriptorSet;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void DescriptorSet::FlushUboUpdates(uint32_t frameIndex)
	{
		PerFrameData& frameData = _perFrameData[frameIndex];

		for (size_t i = 0; i < _uboData.Size(); ++i)
		{
			void* data = frameData.uboBuffers[i]->Lock();
			if (data != nullptr)
			{
				memcpy(data, _uboData[i].Data(), _uboData[i].Size());
				frameData.uboBuffers[i]->Unlock();
			}
		}

		frameData.uboDirty = false;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void DescriptorSet::FlushTextureUpdates(uint32_t frameIndex)
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();
		PerFrameData&   frameData = _perFrameData[frameIndex];

		const Vector<ShaderSetDescriptorVk::BlockTexture>& textures     = _descriptorSetLayout->GetTextureBlocks();
		size_t                                              textureCount = textures.Size();

		for (size_t i = 0; i < textureCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockTexture& texture   = textures[i];
			const VkTexture*                           vkTexture = _currentTextures[i];

			VkDescriptorImageInfo imageInfo = {};
			if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Sampler)
			{
				imageInfo.sampler = vkTexture->GetTextureSampler();
			}
			else if (texture._type == ShaderSetDescriptorVk::BlockTexture::Type::Texture)
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView   = vkTexture->GetTextureImageView();
			}
			else
			{
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView   = vkTexture->GetTextureImageView();
				imageInfo.sampler     = vkTexture->GetTextureSampler();
			}

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet          = frameData.descriptorSet;
			descriptorWrite.dstBinding      = texture._binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType  = ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(texture._type);
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo      = &imageInfo;

			vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		frameData.textureDirty = false;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void DescriptorSet::SetUboValue(const String& memberName, const void* value, size_t valueSize)
	{
		String varIdentifier;
		String target;

		size_t len = memberName.FindFirstOf(".[");
		if (len == String::Npos)
		{
			target        = memberName;
			varIdentifier = "";
		}
		else
		{
			target        = memberName.SubStr(0, len);
			varIdentifier = memberName.SubStr(len);
		}

		const Vector<ShaderSetDescriptorVk::BlockUbo>& ubos     = _descriptorSetLayout->GetUboBlocks();
		size_t                                          uboCount = ubos.Size();

		for (size_t i = 0; i < uboCount; ++i)
		{
			const ShaderSetDescriptorVk::BlockUbo& ubo = ubos[i];

			if (ubo._name != target)
			{
				continue;
			}

			size_t                                         offset = 0;
			const ShaderSetDescriptorVk::BlockUbo::Member* member = &ubo._rootMember;

			String remaining = varIdentifier;
			while (remaining.Empty() == false)
			{
				if (remaining[0] == '.')
				{
					String subMemberName;
					size_t subLen = remaining.FindFirstOf(".[", 1);

					if (subLen == String::Npos)
					{
						subMemberName = remaining.SubStr(1);
						remaining     = "";
					}
					else
					{
						subMemberName = remaining.SubStr(1, subLen - 1);
						remaining     = remaining.SubStr(subLen);
					}

					auto it = member->_childsMap.find(subMemberName);
					if (it == member->_childsMap.end())
					{
						return;
					}

					member  = &it->second;
					offset += member->_offset;
				}
				else if (remaining[0] == '[')
				{
					size_t closeLen = remaining.Find(']', 1);
					String indexStr = remaining.SubStr(1, closeLen - 1);
					int    index    = std::atoi(indexStr.CStr());

					offset   += member->_size * index;
					remaining = remaining.SubStr(closeLen + 1);
				}
			}

			if (member->_size != valueSize)
			{
				OUTPUT_ERROR("SetUboValue: member->Size != valueSize !");
				return;
			}

			// Write to the CPU mirror — all FIF buffers will be lazily synced in GetDescriptorSet()
			memcpy(_uboData[i].Data() + offset, value, valueSize);

			for (PerFrameData& frameData : _perFrameData)
			{
				frameData.uboDirty = true;
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void DescriptorSet::SetTexture(const String& name, const VkTexture* textureSampler)
	{
		const Vector<ShaderSetDescriptorVk::BlockTexture>& textures     = _descriptorSetLayout->GetTextureBlocks();
		size_t                                              textureCount = textures.Size();

		bool found = false;
		for (size_t i = 0; i < textureCount; ++i)
		{
			if (textures[i]._name == name)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			return;
		}

		// Store desired state — all FIF slots will be lazily updated in GetDescriptorSet()
		for (size_t i = 0; i < textureCount; ++i)
		{
			_currentTextures[i] = textureSampler;
		}

		for (PerFrameData& frameData : _perFrameData)
		{
			frameData.textureDirty = true;
		}
	}
}
