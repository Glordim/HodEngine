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

        bool					ConvertTokens(const Vector<Token>& inTokens, Vector<Token>& outTokens) override;
		bool					CompileSource(Vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source) override;
    };
}
