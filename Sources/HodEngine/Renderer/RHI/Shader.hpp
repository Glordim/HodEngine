#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <cstdint>

#include <unordered_map>

namespace hod::renderer
{
	class ShaderSetDescriptor;
	class ShaderConstantDescriptor;

	/// @brief
	class HOD_RENDERER_API Shader
	{
	public:
		enum ShaderType
		{
			Vertex,
			Geometry,
			Fragment,
			Compute
		};

	public:
		Shader(ShaderType type);
		virtual ~Shader();

		const Vector<uint8_t>& GetShaderBytecode() const;

		ShaderType GetShaderType() const;

		virtual bool LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize) = 0;

		const ShaderConstantDescriptor*                           GetConstantDescriptor() const;
		const std::unordered_map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

	protected:
		Vector<uint8_t>                                    _buffer;
		ShaderConstantDescriptor*                          _constantDescriptor;
		std::unordered_map<uint32_t, ShaderSetDescriptor*> _setDescriptors;

	private:
		ShaderType _type;
	};
}
