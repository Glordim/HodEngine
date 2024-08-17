#pragma once
#include "HodEngine/Renderer/Export.hpp"

#if defined(_WIN32)

#include "HodEngine/Renderer/RHI/Shader.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API D3dShader : public Shader
		{
		public:
											D3dShader(ShaderType type);
											~D3dShader() override;

			//bool							LoadFromMemory(void* data, uint32_t size) override;
		};
	}
}

#endif
