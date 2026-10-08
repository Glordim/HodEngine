#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/ShaderConstantDescriptor.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
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
		DefaultAllocator::GetInstance().Delete(_constantDescriptor);

		for (const auto& pair : _setDescriptors)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief 
	/// @return 
	const Vector<uint8_t>& Shader::GetShaderBytecode() const
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
