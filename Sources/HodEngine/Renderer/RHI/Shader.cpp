#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/Renderer/RHI/ShaderConstantDescriptor.hpp"

#include <vector>
#include <fstream>
#include <iostream>

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	/// @brief 
	/// @param type 
	Shader::Shader(ShaderType type)
	{
		_type = type;
		_constantDescriptor = nullptr;
	}

	/// @brief 
	Shader::~Shader()
	{
		delete _constantDescriptor;

		for (const auto& pair : _setDescriptors)
		{
			delete pair.second;
		}
	}

	/// @brief 
	/// @return 
	const std::vector<uint8_t>& Shader::GetShaderBytecode() const
	{
		return _buffer;
	}

	/// @brief 
	/// @return 
	Shader::ShaderType Shader::GetShaderType() const
	{
		return _type;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<uint32_t, ShaderSetDescriptor*>& Shader::GetSetDescriptors() const
	{
		return _setDescriptors;
	}

	/// @brief 
	/// @return 
	const ShaderConstantDescriptor* Shader::GetConstantDescriptor() const
	{
		return _constantDescriptor;
	}
}
