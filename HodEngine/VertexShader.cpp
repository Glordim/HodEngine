#include "VertexShader.h"

VertexShader::VertexShader() : Shader()
{
}

VertexShader::~VertexShader()
{
}

bool VertexShader::load(const char* path)
{
    return this->load_internal(GL_VERTEX_SHADER, path);
}
