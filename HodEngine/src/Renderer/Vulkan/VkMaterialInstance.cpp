#include "VkMaterialInstance.h"
#include "VkMaterial.h"
#include "VkShader.h"

#include "RendererVulkan.h"

#include "../Mesh.h"

VkMaterialInstance::VkMaterialInstance() : MaterialInstance()
{
    this->material = nullptr;

    this->uniformBuffer = VK_NULL_HANDLE;
    this->uniformBufferMemory = VK_NULL_HANDLE;

    this->descriptorSet = VK_NULL_HANDLE;
}

VkMaterialInstance::~VkMaterialInstance()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (this->descriptorSet != VK_NULL_HANDLE)
        vkFreeDescriptorSets(renderer->GetVkDevice(), renderer->GetDescriptorPool(), 1, &this->descriptorSet);

    if (this->uniformBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(renderer->GetVkDevice(), this->uniformBuffer, nullptr);
    if (this->uniformBufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(renderer->GetVkDevice(), this->uniformBufferMemory, nullptr);
}

bool VkMaterialInstance::SetMaterial(Material* material)
{
    if (this->material == material)
        return true;

    this->material = (VkMaterial*)material;

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    VkDescriptorSetLayout descriptorLayout = this->material->GetDescriptorLayout();

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderer->GetDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorLayout;

    if (vkAllocateDescriptorSets(renderer->GetVkDevice(), &allocInfo, &this->descriptorSet) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to allocate descriptor sets!\n");
        return false;
    }

    if (renderer->CreateBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->uniformBuffer, &this->uniformBufferMemory) == false)
        return false;

    return true;
}

void VkMaterialInstance::UpdateUbo(UniformBufferObject ubo)
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    void* data = nullptr;

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    if (vkMapMemory(renderer->GetVkDevice(), this->uniformBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to map ubo buffer memory!\n");
        return;
    }
    memcpy(data, &ubo, (size_t)bufferSize);
    vkUnmapMemory(renderer->GetVkDevice(), this->uniformBufferMemory);

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = this->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = bufferSize;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = this->descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(renderer->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
}

VkMaterial* VkMaterialInstance::GetMaterial() const
{
    return this->material;
}

VkDescriptorSet VkMaterialInstance::GetDescriptorSet() const
{
    return this->descriptorSet;
}