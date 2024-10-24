#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3dShader.hpp"

#if defined(_WIN32)

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		D3dShader::D3dShader(Shader::ShaderType type) : Shader(type)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		D3dShader::~D3dShader()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		/*
		bool D3dShader::LoadFromMemory(void* data, uint32_t size)
		{
			return false;
		}
		*/
	}
}

#endif
