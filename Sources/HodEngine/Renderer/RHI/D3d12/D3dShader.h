#pragma once

#if defined(_WIN32)

#include "HodEngine/Renderer/RHI/Shader.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class D3dShader : public Shader
		{
		public:
											D3dShader(ShaderType type);
											~D3dShader() override;

			bool							LoadFromMemory(void* data, uint32_t size) override;
		};
	}
}

#endif
