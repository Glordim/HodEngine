#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Shader.hpp"

#include <cstdint>

namespace hod::inline rhi
{
	/// @brief Push constant block of a shader, built from its reflection
	class HOD_RHI_API ShaderConstantDescriptor
	{
	public:
		ShaderConstantDescriptor(uint32_t offset, uint32_t size, Shader::ShaderType shaderType);
		~ShaderConstantDescriptor() = default;

		uint32_t           GetOffset() const;
		uint32_t           GetSize() const;
		Shader::ShaderType GetShaderType() const;

	private:
		uint32_t           _offset = 0;
		uint32_t           _size = 0;
		Shader::ShaderType _shaderType;
	};
}
