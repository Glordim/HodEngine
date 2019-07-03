#ifndef __VK_MATERIAL_HPP__
#define __VK_MATERIAL_HPP__

#include "Material.h"

#include <vulkan.h>

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 mvp;
};

class VkMaterial : public Material
{
public:

    VkMaterial();
    virtual ~VkMaterial();

    virtual bool Build(Shader* vertexShader, Shader* fragmentShader) override;

    VkPipeline GetGraphicsPipeline() const;

    void UpdateUbo(UniformBufferObject ubo);

private:
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;

    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
};

#endif
