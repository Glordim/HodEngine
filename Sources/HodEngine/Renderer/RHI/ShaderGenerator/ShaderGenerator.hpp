#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <vector>
#include <string>
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
	
		bool				GenerateByteCode(std::vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source);
		bool				GenerateSource(std::string& generatedSource, std::string_view source);

		virtual bool		ConvertTokens(const std::vector<Token>& inTokens, std::vector<Token>& outTokens) = 0;
		virtual bool		CompileSource(std::vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source) = 0;

	protected:

		bool				NextTokensAre(const std::vector<Token>& tokens, uint32_t& index, const std::vector<Token>& expectedTokens, std::vector<std::string>* identifiers = nullptr);
    };
}
