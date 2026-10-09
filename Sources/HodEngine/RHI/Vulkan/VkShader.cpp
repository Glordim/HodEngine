#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <cstring>
#include <string_view>

#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#undef min
#undef max

namespace hod::inline rhi
{
	/// @brief
	/// @param type
	VkShader::VkShader(ShaderType type)
	: Shader(type)
	{
	}

	/// @brief
	VkShader::~VkShader()
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		if (_shaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(rhiDevice->GetVkDevice(), _shaderModule, nullptr);
		}
	}

	/// @brief
	/// @param data
	/// @param Size
	/// @return
	bool VkShader::LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pNext = nullptr;
		createInfo.codeSize = bytecodeSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode);

		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		if (vkCreateShaderModule(rhiDevice->GetVkDevice(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		{
			OUTPUT_ERROR("VkShader : Failed to create Shader Module");
			return false;
		}

		_buffer.Resize(bytecodeSize);
		memcpy(_buffer.Data(), bytecode, bytecodeSize);

		if (GenerateDescriptors(reflection, reflectionSize) == false)
		{
			vkDestroyShaderModule(rhiDevice->GetVkDevice(), _shaderModule, nullptr);
			_shaderModule = VK_NULL_HANDLE;

			OUTPUT_ERROR("VkShader : Failed to extract descriptor");
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	VkShaderModule VkShader::GetShaderModule() const
	{
		return _shaderModule;
	}
}
