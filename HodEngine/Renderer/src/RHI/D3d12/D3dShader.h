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
			virtual ~D3dShader();

			virtual bool LoadFromFile(const std::string& path) override;
		};
	}
}

#endif
