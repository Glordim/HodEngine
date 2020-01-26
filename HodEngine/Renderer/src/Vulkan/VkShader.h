#pragma once

#include "../Shader.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace HOD
{
    class VkShader : public Shader
    {
    public:
        VkShader(ShaderType type);
        virtual ~VkShader();

        virtual bool LoadFromFile(const std::string& path) override;

        VkShaderModule GetShaderModule() const;

        const std::vector<uint32_t>& GetShaderBytecode() const;

    private:
        VkShaderModule shaderModule;

        std::vector<uint32_t> buffer;
    };
}
