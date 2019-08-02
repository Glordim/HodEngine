#ifndef __D3D_SHADER_HPP__
#define __D3D_SHADER_HPP__

#include "../Shader.h"

class D3dShader : public Shader
{
public:
    D3dShader(ShaderType type);
    virtual ~D3dShader();

    virtual bool LoadFromFile(const std::string& path) override;
};

#endif
