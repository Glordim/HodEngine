#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Shader.hpp"

#include "HodEngine/Core/String.hpp"
#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API VulkanShader : public Shader
	{
	public:
		VulkanShader(ShaderType type);
		~VulkanShader() override;

		VkShaderModule GetShaderModule() const;

	protected:
		bool LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize) override;

	private:
		VkShaderModule _shaderModule = VK_NULL_HANDLE;
	};
}
