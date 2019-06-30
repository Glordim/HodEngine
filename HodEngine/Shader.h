#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <string>

class Shader
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

    virtual bool LoadFromFile(const std::string& path) = 0;

private:
    ShaderType type;
};

#endif
