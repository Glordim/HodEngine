#include "HodEngine/Renderer/RHI/Shader.h"

#include <vector>
#include <fstream>
#include <iostream>

#include <HodEngine/Core/Output.h>
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Stream/MemoryStream.h"

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
		bool Shader::LoadFromSource(Stream& stream)
		{
			return true;
		}

		/// @brief 
		/// @param path 
		/// @return 
		bool Shader::LoadFromFile(const std::string& path)
		{
			FileStream fileStream(path, FileMode::Read);
			/*
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
		bool Shader::LoadFromStream(Stream& stream)
		{

			return true;
		}

		/// @brief 
		/// @return 
		const std::vector<uint8_t>& Shader::GetShaderBytecode() const
		{
			return _buffer;
		}
	}
}
