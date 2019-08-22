#ifndef __VK_MATERIAL_HPP__
#define __VK_MATERIAL_HPP__

#include "../Material.h"

#include <vulkan.h>

#include "DescriptorSetLayout.h"

class VkMaterial : public Material
{
public:

    VkMaterial();
    virtual ~VkMaterial();

    virtual bool Build(Shader* vertexShader, Shader* fragmentShader, Topololy topololy = Topololy::TRIANGLE) override;

    VkPipeline GetGraphicsPipeline() const;
    VkPipelineLayout GetPipelineLayout() const;

    const std::vector<DescriptorSetLayout>& GetDescriptorSetLayouts() const;

private:
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<DescriptorSetLayout> descriptorSetLayouts;
};

#endif
