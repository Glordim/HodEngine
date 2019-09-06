#ifndef __DESCRIPTOR_SET_HPP__
#define __DESCRIPTOR_SET_HPP__

#include <vulkan.h>

#include <vector>

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

    void SetUboValue(const std::string& uboName, const std::string& memberName, const void* value);
    void SetUboValueInArray(const std::string& uboName, const std::string& memberName, size_t index, const void* value);
    void SetTexture(const std::string& name, const VkTexture* textureSampler);

private:

    const DescriptorSetLayout* descriptorSetLayout;

    VkDescriptorSet descriptorSet;

    std::vector<VkBuffer> uboBuffers;
    std::vector<VkDeviceMemory> uboBufferMemories;
};

#endif
