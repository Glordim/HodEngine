#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(_WIN32)

#include "HodEngine/Renderer/RHI/Shader.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API D3dShader : public Shader
		{
		public:
											D3dShader(ShaderType type);
											~D3dShader() override;

			//bool							LoadFromMemory(void* data, uint32_t size) override;
		};
	}
}

#endif
