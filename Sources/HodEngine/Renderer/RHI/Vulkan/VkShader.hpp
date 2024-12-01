#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <vulkan/vulkan.h>
#include <string>

namespace hod::renderer
{
	class ShaderSetDescriptorVk;

	/// @brief 
	class HOD_RENDERER_API VkShader : public Shader
	{
	public:

									VkShader(ShaderType type);
									~VkShader() override;


		VkShaderModule				GetShaderModule() const;

	protected:

		bool						LoadFromSource(std::string_view source) override;
		bool						LoadFromIR(const void* data, uint32_t size) override;
		bool						GenerateDescriptors() override;

	private:

		ShaderSetDescriptorVk*		GetOrCreateSetDescriptor(uint32_t set);

	private:

		VkShaderModule				_shaderModule = VK_NULL_HANDLE;
	};
}
