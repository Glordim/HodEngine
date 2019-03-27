#include "FragmentShader.h"

FragmentShader::FragmentShader() : Shader()
{
}

FragmentShader::~FragmentShader()
{
}

bool FragmentShader::load(const std::string& path)
{
    return this->load_internal(GL_FRAGMENT_SHADER, path);
}
