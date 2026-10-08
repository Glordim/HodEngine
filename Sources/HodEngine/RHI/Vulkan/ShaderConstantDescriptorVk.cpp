#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/ShaderConstantDescriptorVk.hpp"

#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"

#include <cassert>
#include <HodEngine/Core/Output/OutputService.hpp>

#undef min
#undef max

namespace hod::inline rhi
{
	/// @brief
	ShaderConstantDescriptorVk::ShaderConstantDescriptorVk(uint32_t offset, uint32_t Size, Shader::ShaderType shaderType)
	{
		switch (shaderType)
		{
			case Shader::ShaderType::Vertex: _pushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT; break;

			case Shader::ShaderType::Fragment: _pushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT; break;

			default:
				// Todo
				assert(false);
				break;
		}

		_pushConstantRange.offset = offset;
		_pushConstantRange.size = Size;
	}

	/// @brief
	ShaderConstantDescriptorVk::~ShaderConstantDescriptorVk() {}

	/// @brief
	/// @return
	VkPushConstantRange ShaderConstantDescriptorVk::GetPushConstantRange() const
	{
		return _pushConstantRange;
	}
}
