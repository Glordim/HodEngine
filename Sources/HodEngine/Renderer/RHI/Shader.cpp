#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <vector>
#include <fstream>
#include <iostream>

#include <HodEngine/Core/Output.hpp>

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
		/// @param stream 
		/// @return 
		bool Shader::LoadFromSource(std::istream& stream)
		{
			return true;
		}

		/// @brief 
		/// @param path 
		/// @return 
		bool Shader::LoadFromFile(const std::string& path)
		{
			std::ifstream fileStream(path);
			/*
			if (fileStream.CanRead() == false)
			{
				OUTPUT_ERROR("VkShader : Failed to load Shader at path: \"{}\"\n", path.c_str());
				return false;
			}
			
			size_t fileSize = fileStream.GetSize();
			_buffer.resize(fileSize);

			if (fileStream.Read(reinterpret_cast<char*>(_buffer.data()), fileSize) == false)
			{
				OUTPUT_ERROR("VkShader : Failed to read Shader at path: \"{}\"\n", path.c_str());
				return false;
			}
			fileStream.Close();
			*/

			return LoadFromStream(fileStream);
			//
		}

		/// @brief 
		/// @param data 
		/// @param size 
		/// @return 
		bool Shader::LoadFromMemory(const void* data, uint32_t size)
		{
			return LoadInternal(data, size);
		}

		/// @brief 
		/// @param stream 
		/// @return 
		bool Shader::LoadFromStream(std::istream& stream)
		{

			return true;
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
