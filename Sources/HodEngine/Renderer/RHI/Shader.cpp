#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <vector>
#include <fstream>
#include <iostream>

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		/// @param type 
		Shader::Shader(ShaderType type)
		{
			_type = type;
		}

		/// @brief 
		Shader::~Shader()
		{

		}

		/// @brief 
		/// @return 
		const std::vector<uint8_t>& Shader::GetShaderBytecode() const
		{
			return _buffer;
		}
    
        ///
        Shader::ShaderType Shader::GetShaderType() const
        {
            return _type;
        }
	}
}
