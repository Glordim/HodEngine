#ifndef __VK_SHADER_HPP__
#define __VK_SHADER_HPP__

#include "Shader.h"

#include <vulkan.h>

class VkShader : public Shader
{
public:
    VkShader(ShaderType type);
    virtual ~VkShader();

    virtual bool LoadFromFile(const std::string& path) override;

    VkShaderModule GetShaderModule() const;

private:
    VkShaderModule shaderModule;
};

#endif
