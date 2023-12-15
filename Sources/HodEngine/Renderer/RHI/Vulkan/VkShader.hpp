#pragma once

#include "HodEngine/Renderer/RHI/Shader.hpp"

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


			VkShaderModule					GetShaderModule() const;

		protected:

			bool							LoadInternal(const void* data, uint32_t size) override;

		private:

			VkShaderModule					_shaderModule = VK_NULL_HANDLE;
		};
	}
}
