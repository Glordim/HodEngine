#pragma once
#include "HodEngine/RHI/Export.hpp"
#include "HodEngine/RHI/ShaderConstantDescriptor.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API ShaderConstantDescriptorVk : public ShaderConstantDescriptor
	{
	public:

											ShaderConstantDescriptorVk(uint32_t offset, uint32_t size, Shader::ShaderType shaderType);
											~ShaderConstantDescriptorVk() override;

		VkPushConstantRange					GetPushConstantRange() const;

	private:

		VkPushConstantRange					_pushConstantRange;
	};
}
