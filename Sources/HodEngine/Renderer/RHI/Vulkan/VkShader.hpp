#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"

#include "HodEngine/Core/String.hpp"
#include <vulkan/vulkan.h>

namespace hod::renderer
{
	class ShaderSetDescriptorVk;

	/// @brief
	class HOD_RENDERER_API VkShader : public Shader
	{
	public:
		VkShader(ShaderType type);
		~VkShader() override;

		VkShaderModule GetShaderModule() const;

	protected:
		bool LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize) override;

	private:
		bool                   GenerateDescriptors(const char* reflection, uint32_t reflectionSize);
		ShaderSetDescriptorVk* GetOrCreateSetDescriptor(uint32_t set);

	private:
		VkShaderModule _shaderModule = VK_NULL_HANDLE;
	};
}
