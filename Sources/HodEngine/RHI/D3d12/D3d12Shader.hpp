#pragma once
#include "HodEngine/RHI/Export.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline rhi
{
	/// @brief A DXIL shader.
	/// Its sets are the register spaces of its parameters, and the binding of a block is its register:
	/// b for a uniform block, t for a texture, s for a sampler.
	class HOD_RHI_API D3d12Shader : public Shader
	{
	public:
		/// @brief
		struct Register
		{
			uint32_t _index = 0;
			uint32_t _space = 0;

			bool operator==(const Register& other) const = default;
		};

		/// @brief
		struct VertexInputSemantic
		{
			String   _name;
			uint32_t _index = 0;
		};

	public:
		D3d12Shader(ShaderType type);
		~D3d12Shader() override = default;

		bool LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize) override;

		D3D12_SHADER_BYTECODE GetBytecode() const;

		// The constant buffer marked [PushConstant], nullptr when the shader has none
		const Register* GetPushConstantRegister() const;

		// The semantics of the vertex inputs, in the order they are declared: the one of the VertexInputs given to a pipeline
		const Vector<VertexInputSemantic>& GetVertexInputSemantics() const;

	private:
		void ExtractPushConstantRegister(const DocumentNode& reflectionNode);
		void ExtractVertexInputSemantics(const DocumentNode& variableNode);

	private:
		bool     _hasPushConstant = false;
		Register _pushConstantRegister;

		Vector<VertexInputSemantic> _vertexInputSemantics;
	};
}
