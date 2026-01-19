#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Shader.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>

namespace hod::renderer
{
	/// @brief
	/// @param type
	D3d12Shader::D3d12Shader(ShaderType type)
	: Shader(type)
	{
		_bytecode.BytecodeLength = 0;
		_bytecode.pShaderBytecode = nullptr;
	}

	/// @brief
	D3d12Shader::~D3d12Shader()
	{
		if (_bytecode.pShaderBytecode != nullptr)
		{
			DefaultAllocator::GetInstance().Free((void*)_bytecode.pShaderBytecode);
		}
	}

	bool D3d12Shader::LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize)
	{
		if (_bytecode.pShaderBytecode != nullptr)
		{
			DefaultAllocator::GetInstance().Free((void*)_bytecode.pShaderBytecode);
		}

		DocumentReaderJson documentReader;
		if (documentReader.Read(_reflection, reflection, reflectionSize) == false)
		{
			return false;
		}

		_bytecode.BytecodeLength = bytecodeSize;
		_bytecode.pShaderBytecode = DefaultAllocator::GetInstance().Allocate(bytecodeSize);
		std::memcpy((void*)_bytecode.pShaderBytecode, bytecode, bytecodeSize);

		return true;
	}

	bool D3d12Shader::GenerateDescriptors()
	{
		return false; // todo
	}

	const D3D12_SHADER_BYTECODE& D3d12Shader::GetBytecode() const
	{
		return _bytecode;
	}

	const Document& D3d12Shader::GetReflection() const
	{
		return _reflection;
	}
}
