#include "HodEngine/Renderer/RHI/Shader.h"

#include <vector>
#include <fstream>
#include <iostream>

#include <HodEngine/Core/Output.h>
#include "HodEngine/Core/Stream/FileStream.h"

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
			FileStream fileStream(path, FileMode::Read);
			if (fileStream.CanRead() == false)
			{
				OUTPUT_ERROR("VkShader : Failed to load Shader at path: \"%s\"\n", path.c_str());
				return false;
			}
			
			size_t fileSize = fileStream.GetSize();
			_buffer.resize(fileSize);

			if (fileStream.Read(reinterpret_cast<char*>(_buffer.data()), fileSize) == false)
			{
				OUTPUT_ERROR("VkShader : Failed to read Shader at path: \"%s\"\n", path.c_str());
				return false;
			}
			fileStream.Close();

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
