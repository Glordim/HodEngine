#include "VkShader.h"

#include <vector>

#include "RendererVulkan.h"

#include <HodEngine/Core/Src/Output.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkShader::VkShader(ShaderType type) : Shader(type)
		{
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
		bool VkShader::LoadFromMemory(void* data, uint32_t size)
		{
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.codeSize = size;
			createInfo.pCode = reinterpret_cast<uint32_t*>(data);

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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkShaderModule VkShader::GetShaderModule() const
		{
			return _shaderModule;
		}
	}
}
