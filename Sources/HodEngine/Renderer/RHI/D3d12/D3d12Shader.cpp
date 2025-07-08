#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Shader.hpp"

namespace hod::renderer
{
	/// @brief 
	/// @param type 
	D3d12Shader::D3d12Shader(ShaderType type) : Shader(type)
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

	bool D3d12Shader::LoadFromIR(const void* data, uint32_t size)
	{
		if (_bytecode.pShaderBytecode != nullptr)
		{
			DefaultAllocator::GetInstance().Free((void*)_bytecode.pShaderBytecode);
		}

		_bytecode.BytecodeLength = size;
		_bytecode.pShaderBytecode = DefaultAllocator::GetInstance().Allocate(size);
		std::memcpy((void*)_bytecode.pShaderBytecode, data, size);
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
}
