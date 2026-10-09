#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <cstdint>

#include <map>

namespace hod::inline rhi
{
	class ShaderSetDescriptor;
	class ShaderConstantDescriptor;

	/// @brief
	class HOD_RHI_API Shader
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
		const std::map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

	protected:
		bool GenerateDescriptors(const char* reflection, uint32_t reflectionSize);

	private:
		ShaderSetDescriptor* GetOrCreateSetDescriptor(uint32_t set);

	protected:
		Vector<uint8_t>                                    _buffer;
		ShaderConstantDescriptor*                          _constantDescriptor;
		std::map<uint32_t, ShaderSetDescriptor*> _setDescriptors;

	private:
		ShaderType _type;
	};
}
