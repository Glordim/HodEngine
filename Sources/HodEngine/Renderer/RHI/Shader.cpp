#include "HodEngine/Renderer/RHI/Shader.h"

#include <vector>
#include <fstream>
#include <iostream>

#include <HodEngine/Core/Output.h>

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
		/// @param path 
		/// @return 
		bool Shader::LoadFromFile(const std::string& path)
		{
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			if (file.is_open() == false)
			{
				OUTPUT_ERROR("VkShader : Failed to load Shader at path: \"%s\"\n", path.c_str());
				return false;
			}

			size_t fileSize = file.tellg();

			std::vector<uint8_t> buffer;

			buffer.clear();
			buffer.resize(fileSize);

			file.seekg(0);
			file.read(reinterpret_cast<char*>(_buffer.data()), fileSize);
			file.close();

			return LoadFromMemory(_buffer.data(), _buffer.size());
		}

		/// @brief 
		/// @return 
		const std::vector<uint8_t>& Shader::GetShaderBytecode() const
		{
			return _buffer;
		}
	}
}
