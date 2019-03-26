#ifndef __FRAGMENT_SHADER_HPP__
#define __FRAGMENT_SHADER_HPP__

#include "Shader.h"

class FragmentShader : public Shader
{
public:
    FragmentShader();
    virtual ~FragmentShader();

    bool load(const char* path);
};

#endif
