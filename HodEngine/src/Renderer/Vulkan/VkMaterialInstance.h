#ifndef __VK_MATERIAL_INSTANCE_HPP__
#define __VK_MATERIAL_INSTANCE_HPP__

#include "../MaterialInstance.h"

#include <vulkan.h>

class VkMaterial;

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 mvp;
};

class VkMaterialInstance : public MaterialInstance
{
public:

    VkMaterialInstance();
    virtual ~VkMaterialInstance();

    virtual bool SetMaterial(Material* material) override;

    VkMaterial* GetMaterial() const;
    VkDescriptorSet GetDescriptorSet() const;

    void UpdateUbo(UniformBufferObject ubo);

private:

    VkMaterial* material;

    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkDescriptorSet descriptorSet;
};

#endif
