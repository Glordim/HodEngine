#ifndef __VERTEX_SHADER_HPP__
#define __VERTEX_SHADER_HPP__

#include "Shader.h"

class VertexShader : public Shader
{
public:
    VertexShader();
    virtual ~VertexShader();

    bool load(const char* path);
};

#endif
