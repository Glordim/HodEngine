#include "Shader.h"

#include <vector>
#include <fstream>
#include <iostream>

namespace HOD
{
    Shader::Shader(ShaderType type)
    {
        this->type = type;
    }

    Shader::~Shader()
    {

    }
}
