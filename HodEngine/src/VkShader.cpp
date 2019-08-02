#include "VkShader.h"

#include <vector>
#include <fstream>
#include <iostream>

#include "RendererVulkan.h"

VkShader::VkShader(ShaderType type) : Shader(type)
{
    this->shaderModule = VK_NULL_HANDLE;
}

VkShader::~VkShader()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (this->shaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(renderer->GetVkDevice(), this->shaderModule, nullptr);
}

bool VkShader::LoadFromFile(const std::string& path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (file.is_open() == false)
    {
        fprintf(stderr, "VkShader : Failed to load Shader at path: \"%s\"\n", path.c_str());
        return false;
    }

    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (vkCreateShaderModule(renderer->GetVkDevice(), &createInfo, nullptr, &this->shaderModule) != VK_SUCCESS)
    {
        fprintf(stderr, "VkShader : Failed to create Shader Module\n");
        return false;
    }

    return true;
}

VkShaderModule VkShader::GetShaderModule() const
{
    return this->shaderModule;
}
