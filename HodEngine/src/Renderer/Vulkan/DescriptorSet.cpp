#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"

#include "VkTexture.h"

DescriptorSet::DescriptorSet()
{
    this->descriptorSetLayout = nullptr;
    this->descriptorSet = VK_NULL_HANDLE;
}

DescriptorSet::~DescriptorSet()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    size_t uboCount = this->uboBuffers.size();
    for (size_t i = 0; i < uboCount; ++i)
    {
        const VkBuffer buffer = this->uboBuffers[i];
        const VkDeviceMemory bufferMemory = this->uboBufferMemories[i];

        if (buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(renderer->GetVkDevice(), buffer, nullptr);
        if (bufferMemory != VK_NULL_HANDLE)
            vkFreeMemory(renderer->GetVkDevice(), bufferMemory, nullptr);
    }

    if (this->descriptorSet != VK_NULL_HANDLE)
        vkFreeDescriptorSets(renderer->GetVkDevice(), renderer->GetDescriptorPool(), 1, &this->descriptorSet);
}

bool DescriptorSet::SetLayout(const DescriptorSetLayout* layout)
{
    this->descriptorSetLayout = layout;

    VkDescriptorSetLayout vkLayout = this->descriptorSetLayout->GetDescriptorSetLayout();

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderer->GetDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &vkLayout;

    if (vkAllocateDescriptorSets(renderer->GetVkDevice(), &allocInfo, &this->descriptorSet) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to allocate descriptor sets!\n");
        return false;
    }

    const std::vector<DescriptorSetLayout::BlockUbo>& ubos = this->descriptorSetLayout->GetUboBlocks();
    size_t uboCount = ubos.size();

    this->uboBuffers.resize(uboCount, VK_NULL_HANDLE);
    this->uboBufferMemories.resize(uboCount, VK_NULL_HANDLE);

    for (size_t i = 0; i < uboCount; ++i)
    {
        const DescriptorSetLayout::BlockUbo& ubo = ubos[i];

        if (renderer->CreateBuffer(ubo.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->uboBuffers.data() + i, this->uboBufferMemories.data() + i) == false)
            return false;
    }
}

const DescriptorSetLayout* DescriptorSet::GetLayout() const
{
    return this->descriptorSetLayout;
}

VkDescriptorSet DescriptorSet::GetDescriptorSet() const
{
    return this->descriptorSet;
}

void DescriptorSet::SetUboValue(const std::string& uboName, const std::string& memberName, const void* value)
{
    const std::vector<DescriptorSetLayout::BlockUbo>& ubos = this->descriptorSetLayout->GetUboBlocks();
    size_t uboCount = ubos.size();

    for (size_t i = 0; i < uboCount; ++i)
    {
        const DescriptorSetLayout::BlockUbo& ubo = ubos[i];

        if (ubo.name == uboName)
        {
            auto it = ubo.memberNameToMemberMap.find(memberName);

            if (it != ubo.memberNameToMemberMap.end())
            {
                RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

                void* data = nullptr;

                if (vkMapMemory(renderer->GetVkDevice(), this->uboBufferMemories[i], 0, ubo.size, 0, &data) != VK_SUCCESS)
                {
                    fprintf(stderr, "Vulkan: Unable to map ubo buffer memory!\n");
                    return;
                }
                memcpy(((char*)data) + it->second.offset, value, it->second.size);
                vkUnmapMemory(renderer->GetVkDevice(), this->uboBufferMemories[i]);

                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = this->uboBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = ubo.size;

                VkWriteDescriptorSet descriptorWrite = {};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = this->descriptorSet;
                descriptorWrite.dstBinding = ubo.binding;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfo;
                descriptorWrite.pImageInfo = nullptr;
                descriptorWrite.pTexelBufferView = nullptr;
                descriptorWrite.pNext = nullptr;

                vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
            }

            break;
        }
    }
}

void DescriptorSet::SetTexture(const std::string& name, const VkTexture* textureSampler)
{
    const std::vector<DescriptorSetLayout::BlockTexture>& textures = this->descriptorSetLayout->GetTextureBlocks();
    size_t textureCount = textures.size();

    for (size_t i = 0; i < textureCount; ++i)
    {
        const DescriptorSetLayout::BlockTexture& texture = textures[i];

        if (texture.name == name)
        {
            RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureSampler->GetTextureImageView();
            imageInfo.sampler = textureSampler->GetTextureSampler();

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = this->descriptorSet;
            descriptorWrite.dstBinding = texture.binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
