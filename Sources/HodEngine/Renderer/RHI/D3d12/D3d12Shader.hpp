#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <HodEngine/Core/Document/Document.hpp>

#include "d3d12.h"

namespace hod::renderer
{
	class HOD_RENDERER_API D3d12Shader : public Shader
	{
	public:
		D3d12Shader(ShaderType type);
		~D3d12Shader() override;

		bool LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize) override;

		const D3D12_SHADER_BYTECODE& GetBytecode() const;
		const Document&              GetReflection() const;

	protected:
		bool GenerateDescriptors();

	private:
		D3D12_SHADER_BYTECODE _bytecode;
		Document              _reflection;
	};
}
