#ifndef __VK_SHADER_HPP__
#define __VK_SHADER_HPP__

#include "../Shader.h"

#include <vulkan/vulkan.h>

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

#endif
