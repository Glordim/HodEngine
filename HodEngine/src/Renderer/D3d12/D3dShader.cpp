#include "D3dShader.h"

D3dShader::D3dShader(Shader::ShaderType type) : Shader(type)
{
}

D3dShader::~D3dShader()
{
}

bool D3dShader::LoadFromFile(const std::string& path)
{
    return false;
}