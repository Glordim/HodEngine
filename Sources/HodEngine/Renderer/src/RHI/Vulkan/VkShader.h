#pragma once

#include "../Shader.h"

#include <vulkan/vulkan.h>

namespace HOD
{
	namespace RENDERER
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

		private:

			VkShaderModule					_shaderModule = VK_NULL_HANDLE;
		};
	}
}
