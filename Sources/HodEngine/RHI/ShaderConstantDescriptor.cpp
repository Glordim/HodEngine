#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/ShaderConstantDescriptor.hpp"

namespace hod::inline rhi
{
	/// @brief
	/// @param offset
	/// @param size
	/// @param index
	/// @param shaderType
	ShaderConstantDescriptor::ShaderConstantDescriptor(uint32_t offset, uint32_t size, uint32_t index, Shader::ShaderType shaderType)
	: _offset(offset)
	, _size(size)
	, _index(index)
	, _shaderType(shaderType)
	{
	}

	/// @brief
	/// @return
	uint32_t ShaderConstantDescriptor::GetIndex() const
	{
		return _index;
	}

	/// @brief
	/// @return
	uint32_t ShaderConstantDescriptor::GetOffset() const
	{
		return _offset;
	}

	/// @brief
	/// @return
	uint32_t ShaderConstantDescriptor::GetSize() const
	{
		return _size;
	}

	/// @brief
	/// @return
	Shader::ShaderType ShaderConstantDescriptor::GetShaderType() const
	{
		return _shaderType;
	}
}
