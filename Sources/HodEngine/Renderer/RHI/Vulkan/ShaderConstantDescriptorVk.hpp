#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/ShaderConstantDescriptor.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <vulkan/vulkan.h>

namespace spirv_cross
{
	class Compiler;
	struct Resource;
	struct SPIRType;
}

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API ShaderConstantDescriptorVk : public ShaderConstantDescriptor
	{
	public:

											ShaderConstantDescriptorVk(uint32_t offset, uint32_t size, Shader::ShaderType shaderType);
											~ShaderConstantDescriptorVk() override;

		VkPushConstantRange					GetPushConstantRange() const;

	private:

		VkPushConstantRange					_pushConstantRange;
	};
}
