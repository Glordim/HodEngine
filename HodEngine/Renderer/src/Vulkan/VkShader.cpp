#include "VkShader.h"

#include <vector>
#include <fstream>
#include <iostream>

#include "RendererVulkan.h"

#include <Core/Src/Output.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkShader::VkShader(ShaderType type) : Shader(type)
		{
			_shaderModule = VK_NULL_HANDLE;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkShader::~VkShader()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			if (_shaderModule != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(renderer->GetVkDevice(), _shaderModule, nullptr);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool VkShader::LoadFromFile(const std::string& path)
		{
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			if (file.is_open() == false)
			{
				OUTPUT_ERROR("VkShader : Failed to load Shader at path: \"%s\"\n", path.c_str());
				return false;
			}

			size_t fileSize = file.tellg();

			_buffer.clear();
			_buffer.resize(fileSize / sizeof(uint32_t));

			file.seekg(0);
			file.read(reinterpret_cast<char*>(_buffer.data()), fileSize);
			file.close();

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = fileSize;
			createInfo.pCode = _buffer.data();

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			if (vkCreateShaderModule(renderer->GetVkDevice(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
			{
				OUTPUT_ERROR("VkShader : Failed to create Shader Module\n");
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkShaderModule VkShader::GetShaderModule() const
		{
			return _shaderModule;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::vector<uint32_t>& VkShader::GetShaderBytecode() const
		{
			return _buffer;
		}
	}
}
