#pragma once

#include "HodEngine/Renderer/RHI/Shader.h"

#include <vulkan/vulkan.h>
#include <string>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkShader : public Shader
		{
		public:

											VkShader(ShaderType type);
											~VkShader() override;

			bool							LoadFromMemory(void* data, uint32_t size) override;

			VkShaderModule					GetShaderModule() const;

		public:

			static bool						ConvertSourceToGlsl(const std::string_view& source, std::string& result);

		private:

			VkShaderModule					_shaderModule = VK_NULL_HANDLE;
		};
	}
}
