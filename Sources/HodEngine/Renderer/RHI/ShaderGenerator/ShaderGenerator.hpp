#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/String.hpp"
#include <cstdint>

#include "Token.hpp"

namespace hod::renderer
{
    class HOD_RENDERER_API ShaderGenerator
    {
    public:
							ShaderGenerator() = default;
							ShaderGenerator(const ShaderGenerator&) = delete;
							ShaderGenerator(ShaderGenerator&&) = delete;
		virtual				~ShaderGenerator() = default;

		ShaderGenerator&   	operator=(const ShaderGenerator&) = delete;
		ShaderGenerator&    operator=(ShaderGenerator&&) = delete;

	public:
	
		bool				GenerateByteCode(Vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source);
		bool				GenerateSource(String& generatedSource, std::string_view source);

		virtual bool		ConvertTokens(const Vector<Token>& inTokens, Vector<Token>& outTokens) = 0;
		virtual bool		CompileSource(Vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source) = 0;

	protected:

		bool				NextTokensAre(const Vector<Token>& tokens, uint32_t& index, const Vector<Token>& expectedTokens, Vector<std::string>* identifiers = nullptr);
    };
}
