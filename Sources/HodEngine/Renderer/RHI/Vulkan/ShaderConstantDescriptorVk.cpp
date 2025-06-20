#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderConstantDescriptorVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <cassert>

#undef min
#undef max
#include <spirv_cross/spirv_cross.hpp>

namespace hod::renderer
{
	/// @brief 
	ShaderConstantDescriptorVk::ShaderConstantDescriptorVk(uint32_t offset, uint32_t size, Shader::ShaderType shaderType)
	{
		switch (shaderType)
		{
		case Shader::ShaderType::Vertex:
			_pushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
			break;

		case Shader::ShaderType::Fragment:
			_pushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		
		default:
			// Todo
			assert(false);
			break;
		}

		_pushConstantRange.offset = offset;
		_pushConstantRange.size = size;
	}

	/// @brief 
	ShaderConstantDescriptorVk::~ShaderConstantDescriptorVk()
	{
	}

	/// @brief 
	/// @return 
	VkPushConstantRange ShaderConstantDescriptorVk::GetPushConstantRange() const
	{
		return _pushConstantRange;
	}
}
