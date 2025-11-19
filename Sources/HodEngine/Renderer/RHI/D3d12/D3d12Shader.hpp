#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include "d3d12.h"

namespace hod::renderer
{
	class HOD_RENDERER_API D3d12Shader : public Shader
	{
	public:

		D3d12Shader(ShaderType type);
		~D3d12Shader() override;

		bool LoadFromIR(const void* data, uint32_t size) override;

		const D3D12_SHADER_BYTECODE& GetBytecode() const;

	protected:

		bool GenerateDescriptors() override;

	private:

		D3D12_SHADER_BYTECODE _bytecode;
	};
}
