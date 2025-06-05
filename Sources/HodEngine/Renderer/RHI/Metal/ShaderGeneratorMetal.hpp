#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/ShaderGenerator/ShaderGenerator.hpp"

namespace hod::renderer
{
    class HOD_RENDERER_API ShaderGeneratorMetal : public ShaderGenerator
    {
    public:
		ShaderGeneratorMetal() = default;
		ShaderGeneratorMetal(const ShaderGeneratorMetal&) = delete;
		ShaderGeneratorMetal(ShaderGeneratorMetal&&) = delete;
		~ShaderGeneratorMetal() = default;

		ShaderGeneratorMetal&    operator=(const ShaderGeneratorMetal&) = delete;
		ShaderGeneratorMetal&    operator=(ShaderGeneratorMetal&&) = delete;

    public:

        bool					ConvertTokens(const Vector<Token>& inTokens, Vector<Token>& outTokens) override;
		bool					CompileSource(Vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source) override;
    };
}
