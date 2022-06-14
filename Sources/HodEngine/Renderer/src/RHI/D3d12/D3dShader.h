#pragma once

#if defined(_WIN32)

#include "../Shader.h"

namespace HOD
{
	namespace RENDERER
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
