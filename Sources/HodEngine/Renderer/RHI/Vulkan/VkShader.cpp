#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"

#include <vector>
#include <cstring>
#include <string_view>

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/RHI/ShaderGenerator/ShaderGenerator.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	/// @brief 
	/// @param type 
	VkShader::VkShader(ShaderType type) : Shader(type)
	{
	}

	/// @brief 
	VkShader::~VkShader()
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (_shaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(renderer->GetVkDevice(), _shaderModule, nullptr);
		}
	}

	bool VkShader::LoadFromSource(std::string_view source)
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		std::vector<uint8_t> shaderIR;
		if (renderer->GetShaderGenerator()->GenerateByteCode(shaderIR, GetShaderType(), source) == false)
		{
			return false;
		}

		return LoadFromIR(shaderIR.data(), shaderIR.size());
	}

	/// @brief 
	/// @param data 
	/// @param size 
	/// @return 
	bool VkShader::LoadFromIR(const void* data, uint32_t size)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pNext = nullptr;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data);

		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (vkCreateShaderModule(renderer->GetVkDevice(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		{
			OUTPUT_ERROR("VkShader : Failed to create Shader Module");
			return false;
		}

		_buffer.resize(size);
		memcpy(_buffer.data(), data, size);

		return true;
	}

	/// @brief 
	/// @return 
	VkShaderModule VkShader::GetShaderModule() const
	{
		return _shaderModule;
	}
}
