#pragma once

#include "../Shader.h"

namespace HOD
{
    class D3dShader : public Shader
    {
    public:
        D3dShader(ShaderType type);
        virtual ~D3dShader();

        virtual bool LoadFromFile(const std::string& path) override;
    };
}
