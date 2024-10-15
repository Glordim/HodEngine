#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/ShaderGenerator/ShaderGenerator.hpp"

namespace hod::renderer
{
    class HOD_RENDERER_API ShaderGeneratorGLSL : public ShaderGenerator
    {
    public:
								ShaderGeneratorGLSL() = default;
								ShaderGeneratorGLSL(const ShaderGeneratorGLSL&) = delete;
								ShaderGeneratorGLSL(ShaderGeneratorGLSL&&) = delete;
								~ShaderGeneratorGLSL() override = default;

		ShaderGeneratorGLSL&    operator=(const ShaderGeneratorGLSL&) = delete;
		ShaderGeneratorGLSL&    operator=(ShaderGeneratorGLSL&&) = delete;

    public:

        bool					ConvertTokens(const std::vector<Token>& inTokens, std::vector<Token>& outTokens) override;
		bool					CompileSource(std::vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source) override;
    };
}
