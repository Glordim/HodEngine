#pragma once

#include "../Material.h"

#include <vulkan/vulkan.h>

#include "DescriptorSetLayout.h"

#include <map>

namespace HOD
{
    class VkMaterial : public Material
    {
    public:

        VkMaterial();
        virtual ~VkMaterial();

        virtual bool Build(Shader* vertexShader, Shader* fragmentShader, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;

        VkPipeline GetGraphicsPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;

        const std::map<int, DescriptorSetLayout>& GetDescriptorSetLayoutMap() const;

    private:
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        std::map<int, DescriptorSetLayout> descriptorSetLayoutMap;
    };
}