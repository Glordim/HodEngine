#pragma once

#include "HodEngine/Renderer/RHI/Shader.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MetalShader : public Shader
		{
		public:

											MetalShader(ShaderType type);
											~MetalShader() override;

		protected:

			bool							LoadInternal(const void* data, uint32_t size) override;
		};
	}
}
