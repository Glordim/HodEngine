#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "glad/glad.h"

class Shader
{
public:
    Shader();
    virtual ~Shader();

    GLuint getShaderId() const;

protected:
    bool load_internal(GLenum shaderType, const char* path);

private:
    GLuint shaderId;
};

#endif
