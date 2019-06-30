#ifndef __VK_MATERIAL_HPP__
#define __VK_MATERIAL_HPP__

#include "Material.h"

#include <vulkan.h>

class VkMaterial : public Material
{
public:
    VkMaterial();
    virtual ~VkMaterial();

    virtual bool Build(Shader* vertexShader, Shader* fragmentShader) override;

private:
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
};

#endif
