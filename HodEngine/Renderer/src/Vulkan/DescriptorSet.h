#pragma once

#include <Vulkan/vulkan.h>

#include <vector>

namespace HOD
{
    class DescriptorSetLayout;
    class VkTexture;

    class DescriptorSet
    {
    public:
        DescriptorSet();
        virtual ~DescriptorSet();

        bool SetLayout(const DescriptorSetLayout* layout);
        const DescriptorSetLayout* GetLayout() const;

        VkDescriptorSet GetDescriptorSet() const;

        void SetUboValue(const std::string& memberName, const void* value, size_t valueSize);
        void SetTexture(const std::string& name, const VkTexture* textureSampler);

    private:

        const DescriptorSetLayout* descriptorSetLayout;

        VkDescriptorSet descriptorSet;

        std::vector<VkBuffer> uboBuffers;
        std::vector<VkDeviceMemory> uboBufferMemories;
    };
}
